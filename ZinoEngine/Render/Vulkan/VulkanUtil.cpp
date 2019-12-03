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

	if(HAS_FLAG(InUsage, EBufferUsage::VertexBuffer))
	{
		Flags |= vk::BufferUsageFlagBits::eVertexBuffer;
	}

	if(HAS_FLAG(InUsage, EBufferUsage::IndexBuffer))
	{
		Flags |= vk::BufferUsageFlagBits::eIndexBuffer;
	}

	if (HAS_FLAG(InUsage, EBufferUsage::TransferSrc))
	{
		Flags |= vk::BufferUsageFlagBits::eTransferSrc;
	}

	if (HAS_FLAG(InUsage, EBufferUsage::TransferDst))
	{
		Flags |= vk::BufferUsageFlagBits::eTransferDst;
	}

	if (HAS_FLAG(InUsage, EBufferUsage::UniformBuffer))
	{
		Flags |= vk::BufferUsageFlagBits::eUniformBuffer;
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

vk::IndexType VulkanUtil::IndexFormatToVkIndexType(const EIndexFormat& InFormat)
{
	switch(InFormat)
	{
	default:
	case EIndexFormat::Uint16:
		return vk::IndexType::eUint16;
	case EIndexFormat::Uint32:
		return vk::IndexType::eUint32;
	}
}

vk::DescriptorType VulkanUtil::ShaderAttributeTypeToVkDescriptorType(const EShaderAttributeType& InType)
{
	switch(InType)
	{
	default:
	case EShaderAttributeType::UniformBufferStatic:
		return vk::DescriptorType::eUniformBuffer;
	}
}

vk::ShaderStageFlags VulkanUtil::ShaderStageFlagsToVkShaderStageFlags(const EShaderStageFlags& InFlags)
{
	vk::ShaderStageFlags Flags;

	if (HAS_FLAG(InFlags, EShaderStage::Vertex))
	{
		Flags |= vk::ShaderStageFlagBits::eVertex;
	}

	if (HAS_FLAG(InFlags, EShaderStage::Fragment))
	{
		Flags |= vk::ShaderStageFlagBits::eFragment;
	}

	return Flags;
}