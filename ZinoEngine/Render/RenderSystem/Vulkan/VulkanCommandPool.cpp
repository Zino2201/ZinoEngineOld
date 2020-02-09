#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

CVulkanCommandPool::CVulkanCommandPool(CVulkanDevice* InDevice,
	const uint32_t& InFamilyIndex,
	const vk::CommandPoolCreateFlags& InCreateFlags) : 
	CVulkanDeviceResource(InDevice)
{
	vk::CommandPoolCreateInfo PoolInfo(
		InCreateFlags,
		InFamilyIndex);

	CommandPool = Device->GetDevice().createCommandPoolUnique(PoolInfo).value;
	if(!CommandPool)
		LOG(ELogSeverity::Fatal, "Failed to create command pool")
}

CVulkanCommandPool::~CVulkanCommandPool() {}