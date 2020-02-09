#include "VulkanUniformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanRenderSystem.h"
#include "VulkanSwapChain.h"

CVulkanUniformBuffer::CVulkanUniformBuffer(CVulkanDevice* InDevice,
	const SRenderSystemUniformBufferInfos& InInfos) : UniformBufferInfos(InInfos),
	IRenderSystemUniformBuffer(InInfos), CVulkanDeviceResource(InDevice)
{
	g_VulkanRenderSystem->GetSwapChain()->OnSwapChainRecreated.Bind(
		std::bind(&CVulkanUniformBuffer::OnSwapchainRecreated, this));

	Create();
}

CVulkanUniformBuffer::~CVulkanUniformBuffer() {}

void CVulkanUniformBuffer::Create()
{
	Buffers.resize(g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size());
	for (size_t i = 0; i < g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size(); ++i)
	{
		Buffers[i] = g_VulkanRenderSystem->CreateBuffer(SRenderSystemBufferInfos(
				UniformBufferInfos.Size,
				EBufferUsage::UniformBuffer,
				EBufferMemoryUsage::CpuToGpu,
				UniformBufferInfos.bUsePersistentMapping));
	}
}

void CVulkanUniformBuffer::OnSwapchainRecreated()
{
	Buffers.clear();
	Create();
}

void* CVulkanUniformBuffer::Map()
{
	return Buffers[0]->Map();
}

void CVulkanUniformBuffer::Unmap()
{
	Buffers[0]->Unmap();
}

void* CVulkanUniformBuffer::GetMappedMemory() const
{
	return Buffers[0]->GetMappedMemory();
}

CRenderSystemBuffer* CVulkanUniformBuffer::GetBuffer() const
{
	return Buffers[0].get();
}