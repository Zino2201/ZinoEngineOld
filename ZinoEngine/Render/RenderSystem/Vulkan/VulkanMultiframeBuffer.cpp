#include "VulkanMultiframeResources.h"

CVulkanMultiframeBuffer::CVulkanMultiframeBuffer(const SRenderSystemBufferInfos& InInfos)
{
	if(InInfos.MemoryUsage == EBufferMemoryUsage::CpuToGpu
		|| InInfos.MemoryUsage == EBufferMemoryUsage::GpuToCpu)
	{
		Buffers.push_back(g_VulkanRenderSystem->CreateBuffer(InInfos));
		Buffers.push_back(g_VulkanRenderSystem->CreateBuffer(InInfos));
	}
	else
		Buffers.resize(1, g_VulkanRenderSystem->CreateBuffer(InInfos));

	CurrentBuffer = 0;

	if(Buffers.size() < 2)
		g_VulkanRenderSystem->GetFrameCompletedDelegate().Bind(std::bind(
			&CVulkanMultiframeBuffer::OnFrameCompletedDelegate, this));
}

void CVulkanMultiframeBuffer::OnFrameCompletedDelegate()
{
	if(Buffers.size() < 2)
		return;

	/** Swap buffers */
	uint64_t NextBuffer = CurrentBuffer + 1 % Buffers.size();

	CRenderSystemBuffer* NextCpuBuffer = Buffers[NextBuffer].get();

	NextCpuBuffer->Copy(Buffers[CurrentBuffer].get());

	CurrentBuffer = NextBuffer;
}