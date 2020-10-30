#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

ZE_FORCEINLINE vk::Format convert_format(Format in_format)
{
	switch(in_format)
	{
	default:
	case Format::Undefined:
		return vk::Format::eUndefined;
	case Format::D24UnormS8Uint:
		return vk::Format::eD24UnormS8Uint;
	case Format::D32Sfloat:
		return vk::Format::eD32Sfloat;
	case Format::D32SfloatS8Uint:
		return vk::Format::eD32SfloatS8Uint;
	case Format::B8G8R8A8Unorm:
		return vk::Format::eB8G8R8A8Unorm;
	case Format::R8G8B8A8Unorm:
		return vk::Format::eR8G8B8A8Unorm;
	case Format::R32Uint:
		return vk::Format::eR32Uint;
	case Format::R64Uint:
		return vk::Format::eR64Uint;
	case Format::R32G32Sfloat:
		return vk::Format::eR32G32Sfloat;
	case Format::R32G32B32Sfloat:
		return vk::Format::eR32G32B32Sfloat;
	case Format::R32G32B32A32Sfloat:
		return vk::Format::eR32G32B32A32Sfloat;
	case Format::R32G32B32A32Uint:
		return vk::Format::eR32G32B32A32Uint;
	}
}

ZE_FORCEINLINE Format to_format(vk::Format in_format)
{
	switch(in_format)
	{
	default:
	case vk::Format::eUndefined:
		return Format::Undefined;
	case vk::Format::eD24UnormS8Uint:
		return Format::D24UnormS8Uint;
	case vk::Format::eD32Sfloat:
		return Format::D32Sfloat;
	case vk::Format::eD32SfloatS8Uint:
		return Format::D32SfloatS8Uint;
	case vk::Format::eB8G8R8A8Unorm:
		return Format::B8G8R8A8Unorm;
	case vk::Format::eR8G8B8A8Unorm:
		return Format::R8G8B8A8Unorm;
	case vk::Format::eR32Uint:
		return Format::R32Uint;
	case vk::Format::eR64Uint:
		return Format::R64Uint;
	case vk::Format::eR32G32Sfloat:
		return Format::R32G32Sfloat;
	case vk::Format::eR32G32B32Sfloat:
		return Format::R32G32B32Sfloat;
	case vk::Format::eR32G32B32A32Sfloat:
		return Format::R32G32B32A32Sfloat;
	case vk::Format::eR32G32B32A32Uint:
		return Format::R32G32B32A32Uint;
	}
}

ZE_FORCEINLINE VmaMemoryUsage convert_memory_usage(MemoryUsage in_usage)
{
	switch(in_usage)
	{
	default:
		return VMA_MEMORY_USAGE_UNKNOWN;
	case MemoryUsage::CpuOnly:
		return VMA_MEMORY_USAGE_CPU_ONLY;
	case MemoryUsage::GpuOnly:
		return VMA_MEMORY_USAGE_GPU_ONLY;
	case MemoryUsage::CpuToGpu:
		return VMA_MEMORY_USAGE_CPU_TO_GPU;
	case MemoryUsage::GpuToCpu:
		return VMA_MEMORY_USAGE_GPU_TO_CPU;
	}
}

ZE_FORCEINLINE vk::ImageType convert_texture_type(TextureType in_type)
{
	switch(in_type)
	{
	case TextureType::Tex1D:
		return vk::ImageType::e1D;
	case TextureType::Tex2D:
		return vk::ImageType::e2D;
	case TextureType::Tex3D:
		return vk::ImageType::e3D;
	}
}

ZE_FORCEINLINE vk::ImageUsageFlags convert_texture_usage_flags(TextureUsageFlags in_flags)
{
	vk::ImageUsageFlags flags;

	if (in_flags & TextureUsageFlagBits::TransferSrc)
		flags |= vk::ImageUsageFlagBits::eTransferSrc;

	if (in_flags & TextureUsageFlagBits::TransferDst)
		flags |= vk::ImageUsageFlagBits::eTransferDst;

	if(in_flags & TextureUsageFlagBits::ColorAttachment)
		flags |= vk::ImageUsageFlagBits::eColorAttachment;

	if(in_flags & TextureUsageFlagBits::DepthStencilAttachment)
		flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;

	if(in_flags & TextureUsageFlagBits::Sampled)
		flags |= vk::ImageUsageFlagBits::eSampled;

	if(in_flags & TextureUsageFlagBits::ShaderResource)
		flags |= vk::ImageUsageFlagBits::eInputAttachment;

	return flags;
}

ZE_FORCEINLINE vk::SampleCountFlagBits convert_sample_count_bit(SampleCountFlagBits bit)
{
	switch(bit)
	{
	case SampleCountFlagBits::Count1:
		return vk::SampleCountFlagBits::e1;
	case SampleCountFlagBits::Count2:
		return vk::SampleCountFlagBits::e2;
	case SampleCountFlagBits::Count4:
		return vk::SampleCountFlagBits::e4;
	case SampleCountFlagBits::Count8:
		return vk::SampleCountFlagBits::e8;
	case SampleCountFlagBits::Count16:
		return vk::SampleCountFlagBits::e16;
	case SampleCountFlagBits::Count32:
		return vk::SampleCountFlagBits::e32;
	case SampleCountFlagBits::Count64:
		return vk::SampleCountFlagBits::e64;
	}
}

ZE_FORCEINLINE vk::ImageViewType convert_texture_view_type(TextureViewType in_type)
{
	switch(in_type)
	{
	case TextureViewType::Tex1D:
		return vk::ImageViewType::e1D;
	case TextureViewType::Tex2D:
		return vk::ImageViewType::e2D;
	case TextureViewType::Tex3D:
		return vk::ImageViewType::e3D;
	case TextureViewType::TexCube:
		return vk::ImageViewType::eCube;
	}
}

ZE_FORCEINLINE vk::ImageAspectFlags convert_texture_aspect_flags(const TextureAspectFlags& in_flags)
{
	vk::ImageAspectFlags flags;
	
	if (in_flags & TextureAspectFlagBits::Color)
		flags |= vk::ImageAspectFlagBits::eColor;

	if (in_flags & TextureAspectFlagBits::Depth)
		flags |= vk::ImageAspectFlagBits::eDepth;

	if (in_flags & TextureAspectFlagBits::Stencil)
		flags |= vk::ImageAspectFlagBits::eStencil;

	return flags;
}

ZE_FORCEINLINE vk::ImageSubresourceRange convert_texture_subresource_range(const TextureSubresourceRange& in_range)
{
	return vk::ImageSubresourceRange(
		convert_texture_aspect_flags(in_range.aspect_flags),
		in_range.base_mip_level,
		in_range.level_count,
		in_range.base_array_layer,
		in_range.layer_count);
}

ZE_FORCEINLINE vk::ImageLayout convert_texture_layout(const TextureLayout& in_layout)
{
	switch(in_layout)
	{
	default:
	case TextureLayout::Undefined:
		return vk::ImageLayout::eUndefined;
	case TextureLayout::ColorAttachment:
		return vk::ImageLayout::eColorAttachmentOptimal;
	case TextureLayout::DepthReadOnly:
		return vk::ImageLayout::eDepthReadOnlyOptimal;
	case TextureLayout::ShaderReadOnly:
		return vk::ImageLayout::eShaderReadOnlyOptimal;
	case TextureLayout::TransferSrc:
		return vk::ImageLayout::eTransferSrcOptimal;
	case TextureLayout::TransferDst:
		return vk::ImageLayout::eTransferDstOptimal;
	case TextureLayout::Present:
		return vk::ImageLayout::ePresentSrcKHR;
	}
}

ZE_FORCEINLINE vk::PipelineStageFlags convert_pipeline_stage_flags(const PipelineStageFlags& in_flags)
{
	vk::PipelineStageFlags flags;

	if(in_flags & PipelineStageFlagBits::TopOfPipe)
		flags |= vk::PipelineStageFlagBits::eVertexInput;

	if(in_flags & PipelineStageFlagBits::InputAssembler)
		flags |= vk::PipelineStageFlagBits::eVertexInput;

	if(in_flags & PipelineStageFlagBits::VertexShader)
		flags |= vk::PipelineStageFlagBits::eVertexShader;

	if(in_flags & PipelineStageFlagBits::TesselationControlShader)
		flags |= vk::PipelineStageFlagBits::eTessellationControlShader;

	if(in_flags & PipelineStageFlagBits::TesselationEvaluationShader)
		flags |= vk::PipelineStageFlagBits::eTessellationEvaluationShader;

	if(in_flags & PipelineStageFlagBits::GeometryShader)
		flags |= vk::PipelineStageFlagBits::eGeometryShader;
	
	if(in_flags & PipelineStageFlagBits::FragmentShader)
		flags |= vk::PipelineStageFlagBits::eFragmentShader;
	
	if(in_flags & PipelineStageFlagBits::EarlyFragmentTests)
		flags |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
		
	if(in_flags & PipelineStageFlagBits::LateFragmentTests)
		flags |= vk::PipelineStageFlagBits::eLateFragmentTests;

	if(in_flags & PipelineStageFlagBits::ComputeShader)
		flags |= vk::PipelineStageFlagBits::eComputeShader;

	if(in_flags & PipelineStageFlagBits::Transfer)
		flags |= vk::PipelineStageFlagBits::eTransfer;

	if(in_flags & PipelineStageFlagBits::BottomOfPipe)
		flags |= vk::PipelineStageFlagBits::eBottomOfPipe;

	return flags;
}

ZE_FORCEINLINE Result convert_vk_result(const vk::Result& in_result)
{
	switch(in_result)
	{
	case vk::Result::eSuccess:
		return Result::Success;
	case vk::Result::eErrorOutOfDeviceMemory:
		return Result::ErrorOutOfDeviceMemory;
	case vk::Result::eErrorOutOfHostMemory:
		return Result::ErrorOutOfHostMemory;
	case vk::Result::eErrorInitializationFailed:
		return Result::ErrorInitializationFailed;
	default:
		return Result::ErrorUnknown;
	}
}

ZE_FORCEINLINE vk::AttachmentLoadOp convert_load_op(const AttachmentLoadOp& in_load_op)
{
	switch(in_load_op)
	{
	case AttachmentLoadOp::Clear:
		return vk::AttachmentLoadOp::eClear;
	case AttachmentLoadOp::Load:
		return vk::AttachmentLoadOp::eClear;
	case AttachmentLoadOp::DontCare:
		return vk::AttachmentLoadOp::eDontCare;
	}
}

ZE_FORCEINLINE vk::AttachmentStoreOp convert_store_op(const AttachmentStoreOp& in_store_op)
{
	switch(in_store_op)
	{
	case AttachmentStoreOp::Store:
		return vk::AttachmentStoreOp::eStore;
	case AttachmentStoreOp::DontCare:
		return vk::AttachmentStoreOp::eDontCare;
	}
}

ZE_FORCEINLINE vk::Rect2D convert_rect2D(const maths::Rect2D& in_rect)
{
	return vk::Rect2D(vk::Offset2D(in_rect.position.x,
		in_rect.position.y), vk::Extent2D(in_rect.size.x, in_rect.size.y));
}

ZE_FORCEINLINE vk::ShaderStageFlagBits convert_shader_stage_bit(const ShaderStageFlagBits& in_bit)
{
	switch(in_bit)
	{
	case ShaderStageFlagBits::Vertex:
		return vk::ShaderStageFlagBits::eVertex;
	case ShaderStageFlagBits::TesselationControl:
		return vk::ShaderStageFlagBits::eTessellationControl;
	case ShaderStageFlagBits::TesselationEvaluation:
		return vk::ShaderStageFlagBits::eTessellationEvaluation;
	case ShaderStageFlagBits::Fragment:
		return vk::ShaderStageFlagBits::eFragment;
	case ShaderStageFlagBits::Geometry:
		return vk::ShaderStageFlagBits::eGeometry;
	case ShaderStageFlagBits::Compute:
		return vk::ShaderStageFlagBits::eCompute;
	}
}

ZE_FORCEINLINE vk::ShaderStageFlags convert_shader_stage_flags(const ShaderStageFlags& in_flags)
{
	vk::ShaderStageFlags flags;

	if(in_flags & ShaderStageFlagBits::Vertex)
		flags |= vk::ShaderStageFlagBits::eVertex;

	if(in_flags & ShaderStageFlagBits::TesselationEvaluation)
		flags |= vk::ShaderStageFlagBits::eTessellationEvaluation;

	if(in_flags & ShaderStageFlagBits::TesselationControl)
		flags |= vk::ShaderStageFlagBits::eTessellationControl;

	if(in_flags & ShaderStageFlagBits::Geometry)
		flags |= vk::ShaderStageFlagBits::eGeometry;

	if(in_flags & ShaderStageFlagBits::Fragment)
		flags |= vk::ShaderStageFlagBits::eFragment;

	if(in_flags & ShaderStageFlagBits::Compute)
		flags |= vk::ShaderStageFlagBits::eCompute;

	return flags;
}

ZE_FORCEINLINE vk::VertexInputRate convert_vertex_input_rate(const VertexInputRate& in_rate)
{
	switch(in_rate)
	{
	case VertexInputRate::Vertex:
		return vk::VertexInputRate::eVertex;
	case VertexInputRate::Instance:
		return vk::VertexInputRate::eInstance;
	}
}

ZE_FORCEINLINE vk::PrimitiveTopology convert_primitive_topology(const PrimitiveTopology& in_primitive_topology)
{
	switch(in_primitive_topology)
	{
	case PrimitiveTopology::PointList:
		return vk::PrimitiveTopology::ePointList;
	case PrimitiveTopology::LineList:
		return vk::PrimitiveTopology::eLineList;
	case PrimitiveTopology::LineStrip:
		return vk::PrimitiveTopology::eLineStrip;
	case PrimitiveTopology::TriangleList:
		return vk::PrimitiveTopology::eTriangleList;
	case PrimitiveTopology::TriangleStrip:
		return vk::PrimitiveTopology::eTriangleStrip;
	case PrimitiveTopology::TriangleFan:
		return vk::PrimitiveTopology::eTriangleFan;
	case PrimitiveTopology::PatchList:
		return vk::PrimitiveTopology::ePatchList;
	}
}

ZE_FORCEINLINE vk::PolygonMode convert_polygon_mode(const PolygonMode& in_mode)
{
	switch(in_mode)
	{
	case PolygonMode::Line:
		return vk::PolygonMode::eLine;
	case PolygonMode::Fill:
		return vk::PolygonMode::eFill;
	}
}

ZE_FORCEINLINE vk::CullModeFlagBits convert_cull_mode(const CullMode& in_cull_mode)
{
	switch(in_cull_mode)
	{
	case CullMode::None:
		return vk::CullModeFlagBits::eNone;
	case CullMode::Back:
		return vk::CullModeFlagBits::eBack;
	case CullMode::Front:
		return vk::CullModeFlagBits::eFront;
	case CullMode::FrontAndBack:
		return vk::CullModeFlagBits::eFrontAndBack;
	}
}

ZE_FORCEINLINE vk::FrontFace convert_front_face(const FrontFace& in_front_face)
{
	switch(in_front_face)
	{
	case FrontFace::Clockwise:
		return vk::FrontFace::eClockwise;
	case FrontFace::CounterClockwise:
		return vk::FrontFace::eCounterClockwise;
	}
}

ZE_FORCEINLINE vk::CompareOp convert_compare_op(const CompareOp& in_compare_op)
{
	switch(in_compare_op)
	{
	case CompareOp::Never:
		return vk::CompareOp::eNever;
	case CompareOp::Less:
		return vk::CompareOp::eLess;
	case CompareOp::LessOrEqual:
		return vk::CompareOp::eLessOrEqual;
	case CompareOp::NotEqual:
		return vk::CompareOp::eNotEqual;
	case CompareOp::Equal:
		return vk::CompareOp::eEqual;
	case CompareOp::Greater:
		return vk::CompareOp::eGreater;
	case CompareOp::GreaterOrEqual:
		return vk::CompareOp::eGreaterOrEqual;
	case CompareOp::Always:
		return vk::CompareOp::eAlways;
	}
}

ZE_FORCEINLINE vk::StencilOp convert_stencil_op(const StencilOp& in_op)
{
	switch(in_op)
	{
	case StencilOp::Zero:
		return vk::StencilOp::eZero;
	case StencilOp::Keep:
		return vk::StencilOp::eKeep;
	case StencilOp::Invert:
		return vk::StencilOp::eInvert;
	case StencilOp::IncrementAndClamp:
		return vk::StencilOp::eIncrementAndClamp;
	case StencilOp::IncrementAndWrap:
		return vk::StencilOp::eIncrementAndWrap;
	case StencilOp::DecrementAndClamp:
		return vk::StencilOp::eDecrementAndClamp;
	case StencilOp::DecrementAndWrap:
		return vk::StencilOp::eDecrementAndWrap;
	case StencilOp::Replace:
		return vk::StencilOp::eReplace;
	}
}

ZE_FORCEINLINE vk::StencilOpState convert_stencil_op_state(const StencilOpState& in_state)
{
	return vk::StencilOpState(convert_stencil_op(in_state.fail_op),
		convert_stencil_op(in_state.pass_op),
		convert_stencil_op(in_state.depth_fail_op),
		convert_compare_op(in_state.compare_op),
		in_state.compare_mask,
		in_state.write_mask,
		in_state.reference);
}

ZE_FORCEINLINE vk::LogicOp convert_logic_op(const LogicOp& in_logic_op)
{
	switch(in_logic_op)
	{
	case LogicOp::Nor:
		return vk::LogicOp::eNor;
	case LogicOp::NoOp:
		return vk::LogicOp::eNoOp;
	case LogicOp::Or:
		return vk::LogicOp::eOr;
	case LogicOp::OrInverted:
		return vk::LogicOp::eOrInverted;
	case LogicOp::OrReverse:
		return vk::LogicOp::eOrReverse;
	case LogicOp::Xor:
		return vk::LogicOp::eXor;
	case LogicOp::And:
		return vk::LogicOp::eAnd;
	case LogicOp::AndInverted:
		return vk::LogicOp::eAndInverted;
	case LogicOp::AndReverse:
		return vk::LogicOp::eAndReverse;
	case LogicOp::Nand:
		return vk::LogicOp::eNand;
	case LogicOp::Clear:
		return vk::LogicOp::eClear;
	case LogicOp::Copy:
		return vk::LogicOp::eCopy;
	case LogicOp::CopyInverted:
		return vk::LogicOp::eCopyInverted;
	case LogicOp::Equivalent:
		return vk::LogicOp::eEquivalent;
	case LogicOp::Invert:
		return vk::LogicOp::eInvert;
	case LogicOp::Set:
		return vk::LogicOp::eSet;
	}
}

ZE_FORCEINLINE vk::BlendFactor convert_blend_factor(const BlendFactor& in_factor)
{
	switch(in_factor)
	{
	case BlendFactor::Zero:
		return vk::BlendFactor::eZero;
	case BlendFactor::One:
		return vk::BlendFactor::eOne;
	case BlendFactor::SrcColor:
		return vk::BlendFactor::eSrcColor;
	case BlendFactor::OneMinusSrcColor:
		return vk::BlendFactor::eOneMinusSrcColor;
	case BlendFactor::ConstantColor:
		return vk::BlendFactor::eConstantColor;
	case BlendFactor::OneMinusConstantColor:
		return vk::BlendFactor::eOneMinusConstantColor;
	case BlendFactor::DstColor:
		return vk::BlendFactor::eDstColor;
	case BlendFactor::OneMinusDstColor:
		return vk::BlendFactor::eOneMinusDstColor;
	case BlendFactor::SrcAlpha:
		return vk::BlendFactor::eSrcAlpha;
	case BlendFactor::OneMinusSrcAlpha:
		return vk::BlendFactor::eOneMinusSrcAlpha;
	case BlendFactor::ConstantAlpha:
		return vk::BlendFactor::eConstantAlpha;
	case BlendFactor::OneMinusConstantAlpha:
		return vk::BlendFactor::eOneMinusConstantAlpha;
	case BlendFactor::DstAlpha:
		return vk::BlendFactor::eDstAlpha;
	case BlendFactor::OneMinusDstAlpha:
		return vk::BlendFactor::eOneMinusDstAlpha;
	}
}

ZE_FORCEINLINE vk::BlendOp convert_blend_op(const BlendOp& in_blend_op)
{
	switch(in_blend_op)
	{
	case BlendOp::Add:
		return vk::BlendOp::eAdd;
	case BlendOp::Min:
		return vk::BlendOp::eMin;
	case BlendOp::Max:
		return vk::BlendOp::eMax;
	case BlendOp::Substract:
		return vk::BlendOp::eSubtract;
	case BlendOp::ReverseSubstract:
		return vk::BlendOp::eReverseSubtract;
	}
}

ZE_FORCEINLINE vk::ColorComponentFlags convert_color_components_flags(const ColorComponentFlags& in_flags)
{
	vk::ColorComponentFlags flags;

	if(in_flags & ColorComponentFlagBits::R)
		flags |= vk::ColorComponentFlagBits::eR;

	if(in_flags & ColorComponentFlagBits::G)
		flags |= vk::ColorComponentFlagBits::eG;

	if(in_flags & ColorComponentFlagBits::B)
		flags |= vk::ColorComponentFlagBits::eB;
	
	if(in_flags & ColorComponentFlagBits::A)
		flags |= vk::ColorComponentFlagBits::eA;
	
	return flags;
}

ZE_FORCEINLINE vk::DescriptorType convert_descriptor_type(const DescriptorType& in_type)
{
	switch(in_type)
	{
	case DescriptorType::UniformBuffer:
		return vk::DescriptorType::eUniformBuffer;
	case DescriptorType::StorageBuffer:
		return vk::DescriptorType::eStorageBuffer;
	case DescriptorType::SampledTexture:
		return vk::DescriptorType::eSampledImage;
	case DescriptorType::Sampler:
		return vk::DescriptorType::eSampler;
	case DescriptorType::InputAttachment:
		return vk::DescriptorType::eInputAttachment;
	}
}

ZE_FORCEINLINE vk::PipelineBindPoint convert_pipeline_bind_point(const PipelineBindPoint& in_bind_point)
{
	switch(in_bind_point)
	{
	case PipelineBindPoint::Gfx:
		return vk::PipelineBindPoint::eGraphics;
	case PipelineBindPoint::Compute:
		return vk::PipelineBindPoint::eCompute;
	case PipelineBindPoint::Raytracing:
		return vk::PipelineBindPoint::eRayTracingKHR;
	}
}

ZE_FORCEINLINE vk::ImageSubresourceLayers convert_texture_subresource_layers(const TextureSubresourceLayers& in_layers)
{
	return vk::ImageSubresourceLayers(
		convert_texture_aspect_flags(in_layers.aspect_flags),
		in_layers.mip_level,
		in_layers.base_array_layer,
		in_layers.layer_count);
}

ZE_FORCEINLINE vk::Extent3D convert_extent3D(const Extent3D& in_extent)
{
	return vk::Extent3D(in_extent.x, in_extent.y, in_extent.z);
}

ZE_FORCEINLINE vk::Offset3D convert_offset3D(const Offset3D& in_offset)
{
	return vk::Offset3D(in_offset.x, in_offset.y, in_offset.z);
}

ZE_FORCEINLINE vk::Filter convert_filter(const Filter& in_filter)
{
	switch(in_filter)
	{
	case Filter::Linear:
		return vk::Filter::eLinear;
	case Filter::Nearest:
		return vk::Filter::eNearest;
	}
}

ZE_FORCEINLINE vk::SamplerMipmapMode convert_mipmapmode(const Filter& in_mode)
{
	switch (in_mode)
	{
	case Filter::Linear:
		return vk::SamplerMipmapMode::eLinear;
	case Filter::Nearest:
		return vk::SamplerMipmapMode::eNearest;
	}
}

ZE_FORCEINLINE vk::SamplerAddressMode convert_address_mode(const SamplerAddressMode& in_mode)
{
	switch(in_mode)
	{
	case SamplerAddressMode::Repeat:
		return vk::SamplerAddressMode::eRepeat;
	case SamplerAddressMode::MirroredRepeat:
		return vk::SamplerAddressMode::eMirrorClampToEdge;
	case SamplerAddressMode::ClampToEdge:
		return vk::SamplerAddressMode::eClampToEdge;
	case SamplerAddressMode::ClampToBorder:
		return vk::SamplerAddressMode::eClampToBorder;
	}
}

ZE_FORCEINLINE vk::AccessFlags convert_access_flags(const AccessFlags& in_flags)
{
	vk::AccessFlags flags;

	if(in_flags & AccessFlagBits::TransferRead)
		flags |= vk::AccessFlagBits::eTransferRead;

	if (in_flags & AccessFlagBits::TransferWrite)
		flags |= vk::AccessFlagBits::eTransferWrite;

	if (in_flags & AccessFlagBits::ShaderRead)
		flags |= vk::AccessFlagBits::eShaderRead;

	if (in_flags & AccessFlagBits::ShaderWrite)
		flags |= vk::AccessFlagBits::eShaderWrite;

	if (in_flags & AccessFlagBits::HostRead)
		flags |= vk::AccessFlagBits::eHostRead;

	if (in_flags & AccessFlagBits::HostWrite)
		flags |= vk::AccessFlagBits::eHostWrite;

	if (in_flags & AccessFlagBits::MemoryRead)
		flags |= vk::AccessFlagBits::eMemoryRead;

	if (in_flags & AccessFlagBits::MemoryWrite)
		flags |= vk::AccessFlagBits::eMemoryWrite;

	if (in_flags & AccessFlagBits::ColorAttachmentRead)
		flags |= vk::AccessFlagBits::eColorAttachmentRead;

	if (in_flags & AccessFlagBits::ColorAttachmentWrite)
		flags |= vk::AccessFlagBits::eColorAttachmentWrite;

	if (in_flags & AccessFlagBits::DepthStencilAttachmentRead)
		flags |= vk::AccessFlagBits::eDepthStencilAttachmentRead;

	if (in_flags & AccessFlagBits::DepthStencilAttachmentWrite)
		flags |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;

	if (in_flags & AccessFlagBits::InputAttachmentRead)
		flags |= vk::AccessFlagBits::eInputAttachmentRead;

	if (in_flags & AccessFlagBits::UniformRead)
		flags |= vk::AccessFlagBits::eUniformRead;


	return flags;
}

}