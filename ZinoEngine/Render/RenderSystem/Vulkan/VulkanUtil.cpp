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
	case EFormat::D32Sfloat:
		return vk::Format::eD32Sfloat;
	case EFormat::D32SfloatS8Uint:
		return vk::Format::eD32SfloatS8Uint;
	case EFormat::D24UnormS8Uint:
		return vk::Format::eD24UnormS8Uint;
	default:
	case EFormat::R8G8B8A8UNorm:
		return vk::Format::eR8G8B8A8Unorm;
	case EFormat::R32G32Sfloat:
		return vk::Format::eR32G32Sfloat;
	case EFormat::R32G32B32Sfloat:
		return vk::Format::eR32G32B32Sfloat;
	case EFormat::R32G32B32A32Sfloat:
		return vk::Format::eR32G32B32A32Sfloat;
	case EFormat::R64Uint:
		return vk::Format::eR64Uint;
	case EFormat::R32Uint:
		return vk::Format::eR32Uint;
	}
}

EFormat VulkanUtil::VkFormatToFormat(const vk::Format& InFormat)
{
	switch (InFormat)
	{
	case vk::Format::eD32Sfloat:
		return EFormat::D32Sfloat;
	case vk::Format::eD32SfloatS8Uint:
		return EFormat::D32SfloatS8Uint;
	case vk::Format::eD24UnormS8Uint:
		return EFormat::D24UnormS8Uint;
	default:
	case vk::Format::eR8G8B8A8Unorm:
		return EFormat::R8G8B8A8UNorm;
	case vk::Format::eR32G32Sfloat:
		return EFormat::R32G32Sfloat;
	case vk::Format::eR32G32B32Sfloat:
		return EFormat::R32G32B32Sfloat;
	case vk::Format::eR32G32B32A32Sfloat:
		return EFormat::R32G32B32A32Sfloat;
	case vk::Format::eR64Uint:
		return EFormat::R64Uint;
	case vk::Format::eR32Uint:
		return EFormat::R32Uint;
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

	if (HAS_FLAG(InUsage, EBufferUsage::StorageBuffer))
	{
		Flags |= vk::BufferUsageFlagBits::eStorageBuffer;
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

vk::DescriptorType VulkanUtil::ShaderParameterTypeToVkDescriptorType(const EShaderParameterType& InType)
{
	switch(InType)
	{
	default:
	case EShaderParameterType::UniformBuffer:
		return vk::DescriptorType::eUniformBuffer;
	case EShaderParameterType::CombinedImageSampler:
		return vk::DescriptorType::eCombinedImageSampler;
	case EShaderParameterType::StorageBuffer:
		return vk::DescriptorType::eStorageBuffer;
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

vk::ImageType VulkanUtil::TextureTypeToVkImageType(const ETextureType& InType)
{
	switch(InType)
	{
	default:
	case ETextureType::Texture2D:
		return vk::ImageType::e2D;
	}
}

vk::ImageUsageFlags VulkanUtil::TextureUsageFlagsToVkImageUsageFlags(const ETextureUsageFlags& InUsage)
{
	vk::ImageUsageFlags Flags;

	if (HAS_FLAG(InUsage, ETextureUsage::TransferSrc))
	{
		Flags |= vk::ImageUsageFlagBits::eTransferSrc;
	}

	if (HAS_FLAG(InUsage, ETextureUsage::TransferDst))
	{
		Flags |= vk::ImageUsageFlagBits::eTransferDst;
	}

	if (HAS_FLAG(InUsage, ETextureUsage::Sampled))
	{
		Flags |= vk::ImageUsageFlagBits::eSampled;
	}

	if (HAS_FLAG(InUsage, ETextureUsage::DepthStencil))
	{
		Flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
	}

	return Flags;
}

VmaMemoryUsage VulkanUtil::TextureMemoryUsageToVmaMemoryUsage(const ETextureMemoryUsage& InUsage)
{
	switch (InUsage)
	{
	default:
	case ETextureMemoryUsage::CpuOnly:
		return VMA_MEMORY_USAGE_CPU_ONLY;
	case ETextureMemoryUsage::CpuToGpu:
		return VMA_MEMORY_USAGE_CPU_TO_GPU;
	case ETextureMemoryUsage::GpuToCpu:
		return VMA_MEMORY_USAGE_GPU_TO_CPU;
	case ETextureMemoryUsage::GpuOnly:
		return VMA_MEMORY_USAGE_GPU_ONLY;
	}
}

vk::ImageViewType VulkanUtil::TextureTypeToVkImageViewType(const ETextureType& InViewType)
{
	switch(InViewType)
	{
	default:
	case ETextureType::Texture2D:
		return vk::ImageViewType::e2D;
	}
}

vk::Filter VulkanUtil::SamplerFilterToVkFilter(const ESamplerFilter& InFilter)
{
	switch(InFilter)
	{
	default:
	case ESamplerFilter::Linear:
		return vk::Filter::eLinear;
	case ESamplerFilter::Nearest:
		return vk::Filter::eNearest;
	}
}

vk::SamplerMipmapMode VulkanUtil::SamplerFilterToVkSamplerMipmapMode(const ESamplerFilter& InFilter)
{
	switch (InFilter)
	{
	default:
	case ESamplerFilter::Linear:
		return vk::SamplerMipmapMode::eLinear;
	case ESamplerFilter::Nearest:
		return vk::SamplerMipmapMode::eNearest;
	}
}

vk::SamplerAddressMode VulkanUtil::SamplerAddressModeToVkSamplerAddressMode(const ESamplerAddressMode& InMode)
{
	switch (InMode)
	{
	default:
	case ESamplerAddressMode::Repeat:
		return vk::SamplerAddressMode::eRepeat;
	case ESamplerAddressMode::Clamp:
		return vk::SamplerAddressMode::eClampToEdge;
	case ESamplerAddressMode::Mirror:
		return vk::SamplerAddressMode::eMirroredRepeat;
	case ESamplerAddressMode::Border:
		return vk::SamplerAddressMode::eClampToBorder;
	}
}

vk::CompareOp VulkanUtil::ComparisonOpToVkCompareOp(const EComparisonOp& InOp)
{
	switch (InOp)
	{
	default:
	case EComparisonOp::Never:
		return vk::CompareOp::eNever;
	case EComparisonOp::Always:
		return vk::CompareOp::eAlways;
	case EComparisonOp::Less:
		return vk::CompareOp::eLess;
	}
}

/** Blend */
vk::BlendFactor VulkanUtil::BlendFactorToVkBlendFactor(EBlendFactor InFactor)
{
	switch(InFactor)
	{
	default:
	case EBlendFactor::Zero:
		return vk::BlendFactor::eZero;
	case EBlendFactor::One:
		return vk::BlendFactor::eOne;
	case EBlendFactor::SrcAlpha:
		return vk::BlendFactor::eSrcAlpha;
	case EBlendFactor::OneMinusSrcAlpha:
		return vk::BlendFactor::eOneMinusSrcAlpha;
	}
}

vk::BlendOp VulkanUtil::BlendOpToVkBlendOp(EBlendOp InOp)
{
	switch (InOp)
	{
	default:
	case EBlendOp::Add:
		return vk::BlendOp::eAdd;
	}
}

vk::StencilOp VulkanUtil::StencilOpToVkStencilOp(ERenderSystemStencilOp InOp)
{
	switch (InOp)
	{
	default:
	case ERenderSystemStencilOp::Keep: return vk::StencilOp::eKeep;
	case ERenderSystemStencilOp::Zero: return vk::StencilOp::eZero;
	case ERenderSystemStencilOp::Replace: return vk::StencilOp::eReplace;
	case ERenderSystemStencilOp::IncrementAndClamp: return vk::StencilOp::eIncrementAndClamp;
	case ERenderSystemStencilOp::DecrementAndClamp: return vk::StencilOp::eDecrementAndClamp;
	case ERenderSystemStencilOp::Invert: return vk::StencilOp::eInvert;
	case ERenderSystemStencilOp::IncrementAndWrap: return vk::StencilOp::eIncrementAndWrap;
	case ERenderSystemStencilOp::DecrementAndWrap: return vk::StencilOp::eDecrementAndWrap;
	}
}

vk::PolygonMode VulkanUtil::PolygonModeToVkPolygonMode(EPolygonMode InPolygonMode)
{
	switch(InPolygonMode)
	{
	default:
	case EPolygonMode::Fill: return vk::PolygonMode::eFill;
	case EPolygonMode::Line: return vk::PolygonMode::eLine;
	case EPolygonMode::Point: return vk::PolygonMode::ePoint;
	}
}

vk::CullModeFlags VulkanUtil::CullModeToVkCullMode(ECullMode InCullMode)
{
	switch (InCullMode)
	{
	default:
	case ECullMode::None: return vk::CullModeFlagBits::eNone;
	case ECullMode::Back: return vk::CullModeFlagBits::eBack;
	case ECullMode::Front: return vk::CullModeFlagBits::eFront;
	case ECullMode::FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
	}
}

vk::FrontFace VulkanUtil::FrontFaceToVkFrontFace(EFrontFace InFrontFace)
{
	switch (InFrontFace)
	{
	default:
	case EFrontFace::Clockwise: return vk::FrontFace::eClockwise;
	case EFrontFace::CounterClockwise: return vk::FrontFace::eCounterClockwise;
	}
}