#include "VulkanUtil.h"

SVulkanSwapChainSupportDetails VulkanUtil::QuerySwapChainSupport(const vk::PhysicalDevice& InDevice,
	const vk::SurfaceKHR& InSurface)
{
	SVulkanSwapChainSupportDetails Details;

	Details.Capabilities = InDevice.getSurfaceCapabilitiesKHR(InSurface).value;
	Details.Formats = InDevice.getSurfaceFormatsKHR(InSurface).value;
	Details.PresentModes = InDevice.getSurfacePresentModesKHR(InSurface).value;

	return Details;
}

SVulkanQueueFamilyIndices VulkanUtil::GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice,
	const vk::SurfaceKHR& InSurface)
{
	SVulkanQueueFamilyIndices Indices;

	std::vector<vk::QueueFamilyProperties> QueueFamilies = InDevice.getQueueFamilyProperties();

	int i = 0;
	for (const vk::QueueFamilyProperties& QueueFamily : QueueFamilies)
	{
		if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			Indices.Graphics = i;

		if (InDevice.getSurfaceSupportKHR(i, InSurface).value)
			Indices.Present = i;

		if (Indices.IsComplete())
			break;

		i++;
	}

	return Indices;
}

vk::ShaderStageFlagBits VulkanUtil::ShaderStageToVkShaderStage(const EShaderStage& InShaderStage)
{
	switch (InShaderStage)
	{
	default:
	case EShaderStage::Vertex:
		return vk::ShaderStageFlagBits::eVertex;
	case EShaderStage::Fragment:
		return vk::ShaderStageFlagBits::eFragment;
	}
}

vk::VertexInputRate VulkanUtil::VertexInputRateToVkVertexInputRate(const EVertexInputRate& InInputRate)
{
	switch(InInputRate)
	{
	default:
	case EVertexInputRate::Vertex:
		return vk::VertexInputRate::eVertex;
	case EVertexInputRate::Instance:
		return vk::VertexInputRate::eInstance;
	}
}

vk::Format VulkanUtil::FormatToVkFormat(const EFormat& InFormat)
{
	switch(InFormat)
	{
	default:
	case EFormat::R32G32Sfloat:
		return vk::Format::eR32G32Sfloat;
	case EFormat::R32G32B32Sfloat:
		return vk::Format::eR32G32B32Sfloat;
	}
}

vk::BufferUsageFlags VulkanUtil::BufferUsageFlagsToVkBufferUsageFlags(const EBufferUsageFlags& InUsage)
{
	vk::BufferUsageFlags Flags;

	if(InUsage & EBufferUsage::VertexBuffer)
	{
		Flags |= vk::BufferUsageFlagBits::eVertexBuffer;
	}

	if(InUsage & EBufferUsage::IndexBuffer)
	{
		Flags |= vk::BufferUsageFlagBits::eIndexBuffer;
	}

	return Flags;
}

VmaMemoryUsage VulkanUtil::BufferMemoryUsageToVmaMemoryUsage(const EBufferMemoryUsage& InUsage)
{
	switch(InUsage)
	{
	default:
	case EBufferMemoryUsage::CpuOnly:
		return VMA_MEMORY_USAGE_CPU_ONLY;
	case EBufferMemoryUsage::CpuToGpu:
		return VMA_MEMORY_USAGE_CPU_TO_GPU;
	case EBufferMemoryUsage::GpuToCpu:
		return VMA_MEMORY_USAGE_GPU_TO_CPU;
	case EBufferMemoryUsage::GpuOnly:
		return VMA_MEMORY_USAGE_GPU_ONLY;
	}
}