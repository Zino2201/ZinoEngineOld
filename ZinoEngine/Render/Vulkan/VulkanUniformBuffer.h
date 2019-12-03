#pragma once

#include "VulkanDeviceResource.h"
#include "Render/UniformBuffer.h"

class CVulkanBuffer;

/**
 * Vulkan uniform buffer
 */
class CVulkanUniformBuffer : public IUniformBuffer,
	public CVulkanDeviceResource
{
public:
	CVulkanUniformBuffer(CVulkanDevice* InDevice,
		const SUniformBufferInfos& InInfos);
	~CVulkanUniformBuffer();

	virtual void* Map() override;
	virtual void Unmap() override;
	virtual void* GetMappedMemory() const override;
	virtual IBuffer* GetBuffer() const override;
private:
	void Create();
	void OnSwapchainRecreated();
private:
	std::vector<std::shared_ptr<CVulkanBuffer>> Buffers;
	SUniformBufferInfos UniformBufferInfos;
};