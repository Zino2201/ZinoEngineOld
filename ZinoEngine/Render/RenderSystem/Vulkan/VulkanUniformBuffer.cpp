#include "VulkanUniformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanRenderSystem.h"
#include "VulkanSwapChain.h"

CVulkanUniformBuffer::CVulkanUniformBuffer(CVulkanDevice* InDevice,
	const SRenderSystemUniformBufferInfos& InInfos) : UniformBufferInfos(InInfos),
	IRenderSystemUniformBuffer(InInfos), CVulkanDeviceResource(InDevice)
{
	//g_VulkanRenderSystem->GetSwapChain()->OnSwapChainRecreated.Bind(
	//	std::bind(&CVulkanUniformBuffer::OnSwapchainRecreated, this));

	Create();
}

CVulkanUniformBuffer::~CVulkanUniformBuffer() {}

void CVulkanUniformBuffer::Create()
{
	Buffer = g_VulkanRenderSystem->CreateBuffer(SRenderSystemBufferInfos(
		UniformBufferInfos.Size,
		EBufferUsage::UniformBuffer,
		EBufferMemoryUsage::CpuToGpu,
		UniformBufferInfos.bUsePersistentMapping,
		"UniformBuffer"));
}

void CVulkanUniformBuffer::FinishDestroy()
{
	Buffer->Destroy();
}

void CVulkanUniformBuffer::OnSwapchainRecreated()
{
	//Buffers.clear();
	//Create();
}

void* CVulkanUniformBuffer::Map()
{
	return Buffer->Map();
}

void CVulkanUniformBuffer::Unmap()
{
	Buffer->Unmap();
}

void* CVulkanUniformBuffer::GetMappedMemory() const
{
	return Buffer->GetMappedMemory();
}

CRenderSystemBuffer* CVulkanUniformBuffer::GetBuffer() const
{
	return Buffer.get();
}