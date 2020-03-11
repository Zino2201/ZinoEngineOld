#include "VulkanVertexBuffer.h"

CVulkanVertexBuffer::CVulkanVertexBuffer(uint64_t InSize,
	EBufferMemoryUsage InMemoryUsage,
	bool bUsePersistentMapping,
	const std::string& InDebugName) : 
		IRenderSystemVertexBuffer(InSize,
			InMemoryUsage,
			bUsePersistentMapping, InDebugName), 
		CVulkanMultiframeBuffer(
			SRenderSystemBufferInfos(InSize,
				EBufferUsageFlags::VertexBuffer | EBufferUsageFlags::TransferDst,
				InMemoryUsage,
				bUsePersistentMapping,
				InDebugName))
{
	
}

void CVulkanVertexBuffer::FinishDestroy()
{
	CVulkanMultiframeBuffer::Destroy();
}

void CVulkanVertexBuffer::Reset(IRenderSystemVertexBuffer* InOther)
{
	//CVulkanMultiframeBuffer::DestroyFrame();
	//Buffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()] = InOther->GetBuffer();
}