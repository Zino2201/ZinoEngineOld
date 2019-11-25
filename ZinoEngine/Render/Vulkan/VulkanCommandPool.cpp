#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

CVulkanCommandPool::CVulkanCommandPool(CVulkanDevice* InDevice) : 
	CVulkanDeviceResource(InDevice)
{
	vk::CommandPoolCreateInfo PoolInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		Device->GetQueueFamilyIndices().Graphics.value());

	CommandPool = Device->GetDevice().createCommandPoolUnique(PoolInfo);
	if(!CommandPool)
		LOG(ELogSeverity::Fatal, "Failed to create command pool")
}

CVulkanCommandPool::~CVulkanCommandPool() {}