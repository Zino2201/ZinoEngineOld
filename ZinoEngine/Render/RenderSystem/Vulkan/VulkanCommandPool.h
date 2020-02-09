#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"

class CVulkanCommandPool : public CVulkanDeviceResource
{
public:
	CVulkanCommandPool(CVulkanDevice* InDevice,
		const uint32_t& InFamilyIndex,
		const vk::CommandPoolCreateFlags& InCreateFlags 
			= vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	~CVulkanCommandPool();

	const vk::CommandPool& GetCommandPool() const { return *CommandPool; }
private:
	vk::UniqueCommandPool CommandPool;
};