#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"

class CVulkanCommandPool : public CVulkanDeviceResource
{
public:
	CVulkanCommandPool(CVulkanDevice* InDevice,
		const uint32_t& InFamilyIndex);
	~CVulkanCommandPool();

	const vk::CommandPool& GetCommandPool() const { return *CommandPool; }
private:
	vk::UniqueCommandPool CommandPool;
};