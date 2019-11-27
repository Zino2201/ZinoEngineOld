#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

CVulkanCommandPool::CVulkanCommandPool(CVulkanDevice* InDevice,
	const uint32_t& InFamilyIndex) : 
	CVulkanDeviceResource(InDevice)
{
	vk::CommandPoolCreateInfo PoolInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		InFamilyIndex);

	CommandPool = Device->GetDevice().createCommandPoolUnique(PoolInfo).value;
	if(!CommandPool)
		LOG(ELogSeverity::Fatal, "Failed to create command pool")
}

CVulkanCommandPool::~CVulkanCommandPool() {}