#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanQueue.h"
#include <set>
#include <iostream>

CVulkanStagingBufferManager::CVulkanStagingBufferManager(CVulkanDevice* InDevice)
	: Device(InDevice) {}

CVulkanInternalStagingBuffer* CVulkanStagingBufferManager::CreateStagingBuffer(uint64_t InSize,
	vk::BufferUsageFlags InUsageFlags)
{
	CVulkanInternalStagingBuffer* Buffer = new CVulkanInternalStagingBuffer(Device,
		InSize, InUsageFlags);
	StagingBuffers.Add(Buffer);

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
		StagingBuffers.Remove(StagingBufferToDelete);

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
			must(false);
			break;
		}
	}

	Entries.clear();
}

CVulkanDevice::CVulkanDevice(const vk::PhysicalDevice& InPhysicalDevice) :
	PresentQueue(nullptr), DeferredDestructionManager(*this), PipelineManager(*this)
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
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create logical device");

		/** Get queues */
		GraphicsQueue = std::make_unique<CVulkanQueue>(this, QueueFamilyIndices.Graphics.value());
	}

	/** Allocator */
	VmaAllocatorCreateInfo AllocatorInfo = {};
	AllocatorInfo.physicalDevice = static_cast<VkPhysicalDevice>(PhysicalDevice);
	AllocatorInfo.device = static_cast<VkDevice>(*Device);

	vmaCreateAllocator(&AllocatorInfo, &Allocator);
	VmaDestructor = std::make_unique<SVMADestructor>(*this);

	StagingBufferMgr = std::make_unique<CVulkanStagingBufferManager>(this);
	PipelineLayoutMgr = std::make_unique<CVulkanPipelineLayoutManager>(this);

	/** Create device context */
	Context = std::make_unique<CVulkanRenderSystemContext>(this);
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
	std::cout << Output << std::endl;
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