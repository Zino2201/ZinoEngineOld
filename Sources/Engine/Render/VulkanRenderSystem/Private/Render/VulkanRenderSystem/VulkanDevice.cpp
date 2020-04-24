#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanQueue.h"
#include <set>

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

CVulkanDevice::CVulkanDevice(const vk::PhysicalDevice& InPhysicalDevice) :
	PresentQueue(nullptr)
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

	StagingBufferMgr = std::make_unique<CVulkanStagingBufferManager>(this);
	PipelineLayoutMgr = std::make_unique<CVulkanPipelineLayoutManager>(this);

	/** Create device context */
	Context = std::make_unique<CVulkanRenderSystemContext>(this);
}

CVulkanDevice::~CVulkanDevice()
{
#ifdef _DEBUG
	/** Print a output to check for any uncleared resources */
	char* Output;
	vmaBuildStatsString(Allocator, &Output, VK_TRUE);
	LOG(ELogSeverity::Debug, VulkanRS, "%s", Output);
	vmaFreeStatsString(Allocator, Output);
#endif
	vmaDestroyAllocator(Allocator);
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