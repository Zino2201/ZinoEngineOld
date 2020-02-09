#pragma once

#include "VulkanDeviceResource.h"
#include "VulkanBuffer.h"

class CVulkanBuffer;

/**
 * Vulkan uniform buffer
 */
class CVulkanUniformBuffer : public IRenderSystemUniformBuffer,
	public CVulkanDeviceResource
{
public:
	CVulkanUniformBuffer(CVulkanDevice* InDevice,
		const SRenderSystemUniformBufferInfos& InInfos);
	~CVulkanUniformBuffer();

	virtual void* Map() override;
	virtual void Unmap() override;
	virtual void* GetMappedMemory() const override;
	virtual CRenderSystemBuffer* GetBuffer() const override;
	virtual const SRenderSystemUniformBufferInfos& GetInfos() const override { return UniformBufferInfos; }
private:
	void Create();
	void OnSwapchainRecreated();
private:
	std::vector<CRenderSystemBufferPtr> Buffers;
	SRenderSystemUniformBufferInfos UniformBufferInfos;
};