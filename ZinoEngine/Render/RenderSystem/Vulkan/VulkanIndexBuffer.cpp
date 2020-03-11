#include "VulkanIndexBuffer.h"

CVulkanIndexBuffer::CVulkanIndexBuffer(uint64_t InSize,
	EBufferMemoryUsage InMemoryUsage,
	bool bUsePersistentMapping,
	const std::string& InDebugName) :
	IRenderSystemIndexBuffer(InSize,
		InMemoryUsage,
		bUsePersistentMapping, InDebugName),
	CVulkanMultiframeBuffer(
		SRenderSystemBufferInfos(InSize,
			EBufferUsageFlags::IndexBuffer | EBufferUsageFlags::TransferDst,
			InMemoryUsage,
			bUsePersistentMapping,
			InDebugName))
{

}

void CVulkanIndexBuffer::Destroy()
{
	CVulkanMultiframeBuffer::Destroy();
}

void CVulkanIndexBuffer::Reset(IRenderSystemIndexBuffer* InOther)
{
	//CVulkanMultiframeBuffer::DestroyFrame();
	//Buffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()] = InOther->GetBuffer();
}