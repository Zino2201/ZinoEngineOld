#include "VulkanUniformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanRenderSystem.h"
#include "VulkanSwapChain.h"

CVulkanUniformBuffer::CVulkanUniformBuffer(CVulkanDevice* InDevice,
	const SUniformBufferInfos& InInfos) : UniformBufferInfos(InInfos),
	IUniformBuffer(InInfos), CVulkanDeviceResource(InDevice)
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
		Buffers[i] = std::static_pointer_cast<CVulkanBuffer>(
			g_VulkanRenderSystem->CreateBuffer(SBufferInfos(
				UniformBufferInfos.Size,
				EBufferUsage::UniformBuffer,
				EBufferMemoryUsage::CpuToGpu,
				UniformBufferInfos.bUsePersistentMapping)));
	}
}

void CVulkanUniformBuffer::OnSwapchainRecreated()
{
	Buffers.clear();
	Create();
}

void* CVulkanUniformBuffer::Map()
{
	return Buffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()]->Map();
}

void CVulkanUniformBuffer::Unmap()
{
	Buffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()]->Unmap();
}

void* CVulkanUniformBuffer::GetMappedMemory() const
{
	return Buffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()]->GetMappedMemory();
}

IBuffer* CVulkanUniformBuffer::GetBuffer() const
{
	return Buffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()].get();
}