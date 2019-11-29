#pragma once

#include "VulkanDeviceResource.h"
#include "Render/Buffer.h"

class CVulkanDevice;

class CVulkanBuffer : public IBuffer,
	public CVulkanDeviceResource
{
public:
	CVulkanBuffer(CVulkanDevice* InDevice,
		const SBufferInfos& InInfos);
	~CVulkanBuffer();

	virtual void* Map() override;
	virtual void Unmap() override;

	const vk::Buffer& GetBuffer() const { return Buffer; }
private:
	vk::Buffer Buffer;
	VmaAllocation Allocation;
};