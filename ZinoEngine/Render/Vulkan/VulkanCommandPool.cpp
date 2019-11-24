#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

CVulkanCommandPool::CVulkanCommandPool(CVulkanDevice* InDevice) : 
	CVulkanDeviceResource(InDevice)
{
	vk::CommandPoolCreateInfo PoolInfo(
		vk::CommandPoolCreateFlags(),
		Device->GetQueueFamilyIndices().Graphics.value());
}

CVulkanCommandPool::~CVulkanCommandPool() {}