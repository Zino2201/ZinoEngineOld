#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanQueue.h"
#include "Render/VulkanRenderSystem/VulkanTexture.h"
#include <set>
#include "Render/RenderSystem/Resources/RenderPass.h"

CVulkanStagingBufferManager::CVulkanStagingBufferManager(CVulkanDevice& InDevice)
	: Device(InDevice) {}

CVulkanInternalStagingBuffer* CVulkanStagingBufferManager::CreateStagingBuffer(uint64_t InSize,
	vk::BufferUsageFlags InUsageFlags)
{
	CVulkanInternalStagingBuffer* Buffer = new CVulkanInternalStagingBuffer(Device,
		InSize, InUsageFlags);
	StagingBuffers.insert(Buffer);

	return Buffer;
}

void CVulkanStagingBufferManager::ReleaseStagingBuffer(CVulkanInternalStagingBuffer* InBuffer)
{
	StagingBuffersToDelete.push_back(InBuffer);
}

void CVulkanStagingBufferManager::ReleaseStagingBuffers()
{
	for(const auto& StagingBufferToDelete : StagingBuffersToDelete)
	{
		StagingBuffers.erase(StagingBufferToDelete);

		delete StagingBufferToDelete;
	}

	StagingBuffersToDelete.clear();
}

/** CVulkanDeferredDestructionMgr */
void CVulkanDeferredDestructionManager::EnqueueResource(EHandleType InHandleType,
	const VmaAllocation& InAllocation, uint64_t InHandle)
{
	Entries.emplace_back(InHandleType, InAllocation, InHandle);
}

void CVulkanDeferredDestructionManager::EnqueueResource(EHandleType InHandleType,
	uint64_t InHandle)
{
	Entries.emplace_back(InHandleType, InHandle);
}

void CVulkanDeferredDestructionManager::DestroyResources()
{
	for(const auto& Entry : Entries)
	{
		switch(Entry.Type)
		{
		case EHandleType::Buffer:
			vmaDestroyBuffer(Device.GetAllocator(),
				reinterpret_cast<VkBuffer>(Entry.Handle),
				Entry.Allocation);
			break;
		case EHandleType::Image:
			vmaDestroyImage(Device.GetAllocator(),
				reinterpret_cast<VkImage>(Entry.Handle),
				Entry.Allocation);
			break;
		case EHandleType::ImageView:
			Device.GetDevice().destroyImageView(
				reinterpret_cast<VkImageView>(Entry.Handle));
			break;
		case EHandleType::Sampler:
			Device.GetDevice().destroySampler(
				reinterpret_cast<VkSampler>(Entry.Handle));
			break;
		default:
			must(false); // Not implemented
			break;
		}
	}

	Entries.clear();
}

/** Render pass manager */
CVulkanRenderPassFramebufferManager::CVulkanRenderPassFramebufferManager(CVulkanDevice& InDevice)
	: Device(InDevice) {}

CVulkanRenderPassFramebufferManager::~CVulkanRenderPassFramebufferManager() = default;

void CVulkanRenderPassFramebufferManager::NewFrame()
{
	/** Increment lifetime counter and destroy unused framebuffers */
	{
		auto It = Framebuffers.begin();
		while (It != Framebuffers.end())
		{
			if (It->second.LifetimeCounter + 1 > GMaxLifetimeFramebuffer)
			{
				It = Framebuffers.erase(It);
			}
			else
			{
				It->second.LifetimeCounter++;
				++It;
			}
		}
	}

	/** Increment lifetime counter and destroy unused render passes */
	{
		auto It = RenderPasses.begin();
		while (It != RenderPasses.end())
		{
			if (It->second.LifetimeCounter + 1 > GMaxLifetimeRenderPass)
			{
				It = RenderPasses.erase(It);
			}
			else
			{
				It->second.LifetimeCounter++;
				++It;
			}
		}
	}
}

vk::RenderPass CVulkanRenderPassFramebufferManager::GetRenderPass(const SRSRenderPass& InRenderPass)
{
	auto FoundRenderPass = RenderPasses.find(InRenderPass);

	if(FoundRenderPass != RenderPasses.end())
	{
		FoundRenderPass->second.LifetimeCounter = 0;
		return *FoundRenderPass->second.RenderPass;
	}
	else
	{
		/** Create render pass */
		std::vector<vk::AttachmentDescription> Attachments;
		Attachments.reserve(InRenderPass.ColorAttachments.size());

		for(const auto& ColorAttachment : InRenderPass.ColorAttachments)
		{
			Attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),
				VulkanUtil::FormatToVkFormat(ColorAttachment.Format),
				VulkanUtil::SampleCountToVkSampleCount(ColorAttachment.SampleCount),
				VulkanUtil::RenderPass::AttachmentLoadOpToVkAttachmentLoadOp(ColorAttachment.Load),
				VulkanUtil::RenderPass::AttachmentStoreOpToVkAttachmentStoreOp(ColorAttachment.Store),
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(ColorAttachment.InitialLayout),
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(ColorAttachment.FinalLayout));
		}

		for (const auto& DepthAttachment : InRenderPass.DepthAttachments)
		{
			Attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),
				VulkanUtil::FormatToVkFormat(DepthAttachment.Format),
				VulkanUtil::SampleCountToVkSampleCount(DepthAttachment.SampleCount),
				VulkanUtil::RenderPass::AttachmentLoadOpToVkAttachmentLoadOp(DepthAttachment.Load),
				VulkanUtil::RenderPass::AttachmentStoreOpToVkAttachmentStoreOp(DepthAttachment.Store),
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(DepthAttachment.InitialLayout),
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(DepthAttachment.FinalLayout));
		}

		std::vector<vk::SubpassDescription> Subpasses;
		Subpasses.reserve(InRenderPass.Subpasses.size());

		/**
		 * We need to keep attachment ref alive for createRenderPass
		 */
		std::vector<vk::AttachmentReference> ColorAttachments;
		std::vector<vk::AttachmentReference> DepthAttachments;
		std::vector<vk::AttachmentReference> InputAttachments;
		size_t FirstIdx = 0;

		for(const auto& Subpass : InRenderPass.Subpasses)
		{
			for(const auto& ColorAttachment : Subpass.ColorAttachmentRefs)
			{
				ColorAttachments.emplace_back(
					ColorAttachment.Index,
					VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
						ColorAttachment.Layout));
			}

			for (const auto& DepthAttachment : Subpass.DepthAttachmentRefs)
			{
				DepthAttachments.emplace_back(
					DepthAttachment.Index,
					VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
						DepthAttachment.Layout));
			}

			for (const auto& InputAttachment : Subpass.InputAttachmentRefs)
			{
				InputAttachments.emplace_back(
					InputAttachment.Index,
					VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
						InputAttachment.Layout));
			}

			Subpasses.emplace_back(
				vk::SubpassDescriptionFlags(),
				vk::PipelineBindPoint::eGraphics,
				static_cast<uint32_t>(InputAttachments.size()),
				InputAttachments.data(),
				static_cast<uint32_t>(Subpass.ColorAttachmentRefs.size()),
				ColorAttachments.empty() ? nullptr : &ColorAttachments[FirstIdx],
				nullptr,
				DepthAttachments.empty() ? nullptr : &DepthAttachments[FirstIdx]);

			FirstIdx += Subpass.ColorAttachmentRefs.size();
		}

		vk::RenderPassCreateInfo CreateInfo = vk::RenderPassCreateInfo(
			vk::RenderPassCreateFlags(),
			static_cast<uint32_t>(Attachments.size()),
			Attachments.data(),
			static_cast<uint32_t>(Subpasses.size()),
			Subpasses.data());
		
		vk::UniqueRenderPass RenderPass = Device.GetDevice().createRenderPassUnique(
			CreateInfo).value;
		if(!RenderPass)
			ZE::Logger::Fatal("Failed to create render pass");

		vk::RenderPass RetRenderPass = *RenderPass;
		RenderPasses.insert({ InRenderPass, std::move(RenderPass) });

		return RetRenderPass;
	}
}

vk::Framebuffer CVulkanRenderPassFramebufferManager::GetFramebuffer(const SRSFramebuffer& InFramebuffer,
	const vk::RenderPass& InRenderPass)
{
	/** Search for key or add it */
	SFramebufferEntryKey Key;	
	Key.RenderPass = InRenderPass;
	for(size_t i = 0; i < InFramebuffer.ColorRTs.size(); ++i)
	{
		if(InFramebuffer.ColorRTs[i])
			Key.ColorRTs[i] =
			reinterpret_cast<uint64_t>(
				static_cast<VkImageView>(static_cast<CVulkanTexture*>(InFramebuffer.ColorRTs[i])->GetImageView()));
	}

	for (size_t i = 0; i < InFramebuffer.DepthRTs.size(); ++i)
	{
		if(InFramebuffer.DepthRTs[i])
			Key.DepthRTs[i] = 
				reinterpret_cast<uint64_t>(
					static_cast<VkImageView>(static_cast<CVulkanTexture*>(InFramebuffer.DepthRTs[i])->GetImageView()));
	}

	auto PossibleFramebuffer = Framebuffers.find(Key);
	if (PossibleFramebuffer != Framebuffers.end())
	{
		PossibleFramebuffer->second.LifetimeCounter = 0;
		return *PossibleFramebuffer->second.Framebuffer;
	}

	/** None found, create one */
	uint32_t Width = 0;
	uint32_t Height = 0;

	std::vector<vk::ImageView> Attachments;
	Attachments.reserve(GMaxRenderTargetPerFramebuffer * 2);
	for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
	{
		CRSTexture* RT = InFramebuffer.ColorRTs[i];
		if (!RT)
			continue;

		CVulkanTexture* VkRt =
			static_cast<CVulkanTexture*>(RT);

		Width = std::max(Width, RT->GetCreateInfo().Width);
		Height = std::max(Height, RT->GetCreateInfo().Height);

		Attachments.push_back(VkRt->GetImageView());
	}

	for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
	{
		CRSTexture* RT = InFramebuffer.DepthRTs[i];
		if (!RT)
			continue;

		CVulkanTexture* VkRt =
			static_cast<CVulkanTexture*>(RT);

		Attachments.push_back(VkRt->GetImageView());
	}

	vk::FramebufferCreateInfo CreateInfo = vk::FramebufferCreateInfo(
		vk::FramebufferCreateFlags(),
		InRenderPass,
		static_cast<uint32_t>(Attachments.size()),
		Attachments.data(),
		Width,
		Height,
		1);

	vk::UniqueFramebuffer Framebuffer = Device.GetDevice().createFramebufferUnique(
		CreateInfo).value;
	if (!Framebuffer)
		ZE::Logger::Fatal("Failed to create framebuffer");

	vk::Framebuffer RetFramebuffer = *Framebuffer;

	Framebuffers.insert({ Key, std::move(Framebuffer) });

	return RetFramebuffer;
}

/** Vulkan device */
CVulkanDevice::CVulkanDevice(const vk::PhysicalDevice& InPhysicalDevice) :
	PresentQueue(nullptr), StagingBufferMgr(*this),
	PipelineLayoutMgr(*this), DeferredDestructionManager(*this), PipelineManager(*this),
	RenderPassFramebufferMgr(*this)
{
	PhysicalDevice = InPhysicalDevice;

	QueueFamilyIndices = VulkanUtil::GetQueueFamilyIndices(InPhysicalDevice);

	/**
	 * Create logical device from physical device
	 */
	{
		/** Queue create infos */

		std::set<uint32_t> UniqueQueueFamilies =
		{
			QueueFamilyIndices.Graphics.value()
		};

		vk::PhysicalDeviceFeatures Features = vk::PhysicalDeviceFeatures()
			.setVertexPipelineStoresAndAtomics(VK_TRUE);

		std::vector<vk::DeviceQueueCreateInfo> QueueCreateInfos;

		float QueuePriority = 1.f;
		for (uint32_t QueueFamily : UniqueQueueFamilies)
		{
			QueueCreateInfos.emplace_back(
				vk::DeviceQueueCreateFlags(),
				QueueFamily,
				1,
				&QueuePriority);
		}

		vk::DeviceCreateInfo CreateInfos(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>(QueueCreateInfos.size()),
			QueueCreateInfos.data(),
			GVulkanEnableValidationLayers ? static_cast<uint32_t>(GVulkanValidationLayers.size()) : 0,
			GVulkanEnableValidationLayers ? GVulkanValidationLayers.data() : 0,
			static_cast<uint32_t>(GVulkanRequiredDeviceExtensions.size()),
			GVulkanRequiredDeviceExtensions.data(),
			&Features);

		Device = PhysicalDevice.createDeviceUnique(CreateInfos).value;
		if (!Device)
			ZE::Logger::Fatal("Failed to create logical device");

		/** Get queues */
		GraphicsQueue = std::make_unique<CVulkanQueue>(*this, QueueFamilyIndices.Graphics.value());
	}

	/** Allocator */
	VmaAllocatorCreateInfo AllocatorInfo = {};
	AllocatorInfo.physicalDevice = static_cast<VkPhysicalDevice>(PhysicalDevice);
	AllocatorInfo.device = static_cast<VkDevice>(*Device);

	vmaCreateAllocator(&AllocatorInfo, &Allocator);
	VmaDestructor = std::make_unique<SVMADestructor>(*this);

	/** Create device context */
	Context = std::make_unique<CVulkanRenderSystemContext>(*this);
}

CVulkanDevice::~CVulkanDevice()
{
	DeferredDestructionManager.DestroyResources();

}

CVulkanDevice::SVMADestructor::~SVMADestructor()
{
#ifdef _DEBUG
	/** Print a output to check for any uncleared resources */
	char* Output;
	vmaBuildStatsString(Device.GetAllocator(), &Output, VK_TRUE);
	ZE::Logger::Verbose("{}", Output);
	vmaFreeStatsString(Device.GetAllocator(), Output);
#endif
	vmaDestroyAllocator(Device.GetAllocator());
}

void CVulkanDevice::WaitIdle()
{
	Device->waitIdle();
}

void CVulkanDevice::CreatePresentQueue(const vk::SurfaceKHR& InSurface)
{
	if(!PresentQueue)
	{
		SVulkanQueueFamilyIndices FamilyIndices = 
			VulkanUtil::GetQueueFamilyIndices(PhysicalDevice);

		// TODO: Iterate over each queue families to find one compatible
		// with present
		// for now, use graphics one

		PresentQueue = GraphicsQueue.get();
	}
}