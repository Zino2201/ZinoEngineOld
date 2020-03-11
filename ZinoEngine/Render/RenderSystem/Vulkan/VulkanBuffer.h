#pragma once

#include "VulkanDeviceResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanDevice;

class CVulkanBuffer : public CRenderSystemBuffer,
	public CVulkanDeviceResource
{
public:
	CVulkanBuffer(CVulkanDevice* InDevice,
		const SRenderSystemBufferInfos& InInfos);
	~CVulkanBuffer();

	virtual void* Map() override;
	virtual void Unmap() override;
	virtual void Copy(CRenderSystemBuffer* InSrc) override;
	virtual void* GetMappedMemory() const override;
	virtual void FinishDestroy() override;

	const vk::Buffer& GetBuffer() const { return Buffer; }
private:
	vk::Buffer Buffer;
	VmaAllocation Allocation;
	VmaAllocationInfo AllocationInfo;
};

using CVulkanBufferPtr = boost::intrusive_ptr<CVulkanBuffer>;