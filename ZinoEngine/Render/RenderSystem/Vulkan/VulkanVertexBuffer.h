#pragma once

#include "VulkanCore.h"
#include "VulkanBuffer.h"
#include "VulkanMultiframeResources.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanVertexBuffer : public IRenderSystemVertexBuffer,
	public CVulkanMultiframeBuffer
{
public:
	CVulkanVertexBuffer(uint64_t InSize,
		EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
		bool bUsePersistentMapping = false,
		const std::string& InDebugName = "VertexBuffer");
	
	virtual void Copy(CRenderSystemBuffer* InSrc) override { CVulkanMultiframeBuffer::Copy(InSrc); }
	virtual void* Map() override { return CVulkanMultiframeBuffer::Map(); }
	virtual void Unmap() override { CVulkanMultiframeBuffer::Unmap(); }
	virtual void Reset(IRenderSystemVertexBuffer* InOther) override;
	virtual void FinishDestroy() override;
	virtual CRenderSystemBuffer* GetBuffer() const override { return CVulkanMultiframeBuffer::GetBuffer(); }
};