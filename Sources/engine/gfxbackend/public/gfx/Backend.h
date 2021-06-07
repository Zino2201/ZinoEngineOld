#pragma once

#include "EngineCore.h"
#include "Resource.h"
#include "maths/Rect.h"
#include <array>
#include <span>
#include <variant>
#include "ShaderFormat.h"
#include "BackendInfo.h"

namespace ze::gfx
{
	
enum class Format
{
    Undefined,

    /** Depth 32-bit (signed float) */
    D32Sfloat,

    /** Depth 32-bit (signed float) & stencil 8-bit (unsigned int) */
    D32SfloatS8Uint,

    /** Depth 24-bit (signed float) & stencil 8-bit (unsigned int) */
    D24UnormS8Uint,

    /** RGB 8-bit (unsigned short) */
    R8Unorm,

    /** RGB 8-bit (unsigned short) */
    R8G8B8Unorm,

    /** RGBA 8-bit (unsigned short) */
    R8G8B8A8Unorm,

    /** RGB 8-bit (srgb nonlinear) */
    R8G8B8A8Srgb,

    /** BGR 8-bit (unsigned short) */
    B8G8R8A8Unorm,

    /** RGBA 16-bit (signed float) */
    R16G16B16A16Sfloat,

    /** RG 32-bit (signed float) */
    R32G32Sfloat,

    /** RGB 32-bit (signed float) */
    R32G32B32Sfloat,

    /** RGBA 32-bit (signed float) */
    R32G32B32A32Sfloat,

    /** RGBA 32-bit (unsigned int) */
    R32G32B32A32Uint,

    /** R 32-bit (unsigned int) */
    R32Uint,

    /** R 64-bit (unsigned int) */
    R64Uint,

    /** BC1/DXT1 */
    Bc1RgbUnormBlock,
    Bc1RgbaUnormBlock,
    Bc1RgbSrgbBlock,
    Bc1RgbaSrgbBlock,

    /** BC3/DXT5 */
    Bc3UnormBlock,
    Bc3SrgbBlock,

    /** BC5 */
    Bc5UnormBlock,
    Bc5SnormBlock,

    /** BC6H */
    Bc6HUfloatBlock,
    Bc6HSfloatBlock,

    /** BC7 */
    Bc7UnormBlock,
    Bc7SrgbBlock,
};

/** Utils structures */
struct Offset3D
{
    int32_t x;
    int32_t y;
    int32_t z;

    Offset3D(const int32_t in_x = 0,
             const int32_t in_y = 0,
             const int32_t in_z = 0) : x(in_x), y(in_y), z(in_z) {}
};

struct Extent3D
{
    uint32_t x;
    uint32_t y;
    uint32_t z;

    Extent3D(const uint32_t in_x,
             const uint32_t in_y,
             const uint32_t in_z) : x(in_x), y(in_y), z(in_z) {}
};

}

namespace std
{

ZE_FORCEINLINE std::string to_string(const ze::gfx::Format& in_format)
{
    switch(in_format)
    {
        default:
        case ze::gfx::Format::Undefined:
            return "Undefined";
        case ze::gfx::Format::D24UnormS8Uint:
            return "D24UnormS8Uint";
        case ze::gfx::Format::D32Sfloat:
            return "D32Sfloat";
        case ze::gfx::Format::D32SfloatS8Uint:
            return "D32SfloatS8Uint";
        case ze::gfx::Format::B8G8R8A8Unorm:
            return "B8G8R8A8Unorm";
        case ze::gfx::Format::R8G8B8Unorm:
            return "R8G8B8Unorm";
        case ze::gfx::Format::R8G8B8A8Unorm:
            return "R8G8B8A8Unorm";
        case ze::gfx::Format::R8G8B8A8Srgb:
            return "R8G8B8A8Srgb";
        case ze::gfx::Format::R16G16B16A16Sfloat:
            return "R16G16B16A16Sfloat";
        case ze::gfx::Format::R32Uint:
            return "R32Uint";
        case ze::gfx::Format::R64Uint:
            return "R64Uint";
        case ze::gfx::Format::R32G32Sfloat:
            return "R32G32Sfloat";
        case ze::gfx::Format::R32G32B32Sfloat:
            return "R32G32B32Sfloat";
        case ze::gfx::Format::R32G32B32A32Sfloat:
            return "R32G32B32A32Sfloat";
        case ze::gfx::Format::R32G32B32A32Uint:
            return "R32G32B32A32Uint";
        case ze::gfx::Format::Bc1RgbUnormBlock:
            return "Bc1RgbUnormBlock";
        case ze::gfx::Format::Bc1RgbaUnormBlock:
            return "Bc1RgbaUnormBlock";
        case ze::gfx::Format::Bc1RgbSrgbBlock:
            return "Bc1RgbSrgbBlock";
        case ze::gfx::Format::Bc1RgbaSrgbBlock:
            return "Bc1RgbaSrgbBlock";
        case ze::gfx::Format::Bc3UnormBlock:
            return "Bc3UnormBlock";
        case ze::gfx::Format::Bc3SrgbBlock:
            return "Bc3SrgbBlock";
        case ze::gfx::Format::Bc7UnormBlock:
            return "Bc7UnormBlock";
        case ze::gfx::Format::Bc7SrgbBlock:
            return "Bc7SrgbBlock";
        case ze::gfx::Format::Bc5UnormBlock:
            return "Bc5UnormBlock";
        case ze::gfx::Format::Bc5SnormBlock:
            return "Bc5SnormBlock";
        case ze::gfx::Format::Bc6HUfloatBlock:
            return "Bc6HUfloatBlock";
        case ze::gfx::Format::Bc6HSfloatBlock:
            return "Bc6HSfloatBlock";
    }
}
}

namespace ze
{
ZE_FORCEINLINE std::string to_string_pretty(const ze::gfx::Format& in_format)
{
    switch (in_format)
    {
        default:
        case ze::gfx::Format::Undefined:
            return "Undefined";
        case ze::gfx::Format::D24UnormS8Uint:
            return "Depth 24-bit (unorm) Stencil 8-bit (uint)";
        case ze::gfx::Format::D32Sfloat:
            return "Depth 32-bit (float)";
        case ze::gfx::Format::D32SfloatS8Uint:
            return "Depth 32-bit (float) Stencil 8-bit (uint)";
        case ze::gfx::Format::B8G8R8A8Unorm:
            return "BGRA 8-bit (unorm)";
        case ze::gfx::Format::R8G8B8Unorm:
            return "RGB 8-bit (unorm)";
        case ze::gfx::Format::R8G8B8A8Unorm:
            return "RGBA 8-bit (unorm)";
        case ze::gfx::Format::R8G8B8A8Srgb:
            return "RGBA 8-bit (sRGB)";
        case ze::gfx::Format::R16G16B16A16Sfloat:
            return "RGBA 16-bit (float)";
        case ze::gfx::Format::R32Uint:
            return "R 32-bit (uint)";
        case ze::gfx::Format::R64Uint:
            return "R 64-bit (uint)";
        case ze::gfx::Format::R32G32Sfloat:
            return "RG 32-bit (float)";
        case ze::gfx::Format::R32G32B32Sfloat:
            return "RGB 32-bit (float)";
        case ze::gfx::Format::R32G32B32A32Sfloat:
            return "RGBA 32-bit (float)";
        case ze::gfx::Format::R32G32B32A32Uint:
            return "RGBA 32-bit (uint)";
        case ze::gfx::Format::Bc1RgbUnormBlock:
            return "BC1 RGB";
        case ze::gfx::Format::Bc1RgbaUnormBlock:
            return "BC1 RGBA";
        case ze::gfx::Format::Bc1RgbSrgbBlock:
            return "BC1 RGB (sRGB)";
        case ze::gfx::Format::Bc1RgbaSrgbBlock:
            return "BC1 RGBA (sRGB)";
        case ze::gfx::Format::Bc3UnormBlock:
            return "BC3";
        case ze::gfx::Format::Bc3SrgbBlock:
            return "BC3 (sRGB)";
        case ze::gfx::Format::Bc7UnormBlock:
            return "BC7";
        case ze::gfx::Format::Bc7SrgbBlock:
            return "BC7 (sRGB)";
        case ze::gfx::Format::Bc5UnormBlock:
            return "BC5 (unorm)";
        case ze::gfx::Format::Bc5SnormBlock:
            return "BC5 (snorm)";
        case ze::gfx::Format::Bc6HUfloatBlock:
            return "BC6H (ufloat)";
        case ze::gfx::Format::Bc6HSfloatBlock:
            return "BC6H (sfloat)";
    }
}

}

namespace ze::gfx
{

/**
 * Preferred memory usage
 */
enum class MemoryUsage
{
	CpuOnly,
	GpuOnly,
	CpuToGpu,
	GpuToCpu
};

/** Buffer related types */
enum class BufferUsageFlagBits
{
	VertexBuffer = 1 << 0,
	IndexBuffer = 1 << 1,
	UniformBuffer = 1 << 2,
	StorageBuffer = 1 << 3,
	TransferSrc = 1 << 4,
	TransferDst = 1 << 5
};
ENABLE_FLAG_ENUMS(BufferUsageFlagBits, BufferUsageFlags);

struct BufferCreateInfo
{
	uint64_t size;
	BufferUsageFlags usage;
	MemoryUsage mem_usage; 

	BufferCreateInfo() : size(0), mem_usage(MemoryUsage::CpuOnly) {}
	
	BufferCreateInfo(const uint64_t& in_size, const BufferUsageFlags& in_usage,
		const MemoryUsage& in_mem_usage) :
		size(in_size), usage(in_usage), mem_usage(in_mem_usage) {}
};

/** SwapChain create infos */
struct SwapChainCreateInfo
{
	void* window_handle;
	uint32_t width;
	uint32_t height;

	SwapChainCreateInfo(void* in_window_handle,
		const uint32_t& in_width,
		const uint32_t& in_height) : window_handle(in_window_handle),
		width(in_width), height(in_height) {}
};

/** Textures */
enum class TextureAspectFlagBits
{
	Color = 1 << 0,
	Depth = 1 << 1,
	Stencil = 1 << 2
};
ENABLE_FLAG_ENUMS(TextureAspectFlagBits, TextureAspectFlags);

/** Layout of a texture in memory */
enum class TextureLayout
{
	Undefined,
	ColorAttachment,
	DepthStencilAttachment,
	DepthReadOnly,
	ShaderReadOnly,
	TransferSrc,
	TransferDst,
	Present,
};

enum class TextureType
{
	Tex1D,
	Tex2D,
	Tex3D,
};

enum class TextureViewType
{
	Tex1D,
	Tex2D,
	Tex3D,
	TexCube
};

/**
 * Subresource range of a texture
 */
struct TextureSubresourceRange
{
	TextureAspectFlags aspect_flags;
	uint32_t base_mip_level;
	uint32_t level_count;
	uint32_t base_array_layer;
	uint32_t layer_count;

	TextureSubresourceRange() : base_mip_level(0), level_count(0),
		base_array_layer(0), layer_count(0) {}

	TextureSubresourceRange(TextureAspectFlags in_aspect_flags,
		uint32_t in_base_mip_level,
		uint32_t in_level_count,
		uint32_t in_base_array_layer,
		uint32_t in_layer_count) : aspect_flags(in_aspect_flags),
			base_mip_level(in_base_mip_level),
			level_count(in_level_count),
			base_array_layer(in_base_array_layer),
			layer_count(in_layer_count) {}
};

/**
 * Specify a subresource layers
 */
struct TextureSubresourceLayers
{
	TextureAspectFlags aspect_flags;
	uint32_t mip_level;
	uint32_t base_array_layer;
	uint32_t layer_count;

	TextureSubresourceLayers(TextureAspectFlags in_aspect_flags,
		uint32_t in_mip_level,
		uint32_t in_base_array_layer,
		uint32_t in_layer_count) : aspect_flags(in_aspect_flags),
		mip_level(in_mip_level),
		base_array_layer(in_base_array_layer),
		layer_count(in_layer_count) {}
};

enum class TextureUsageFlagBits
{
	ColorAttachment = 1 << 0,
	DepthStencilAttachment = 1 << 1,
	Sampled = 1 << 2,
	TransferSrc = 1 << 3,
	TransferDst = 1 << 4,
};
ENABLE_FLAG_ENUMS(TextureUsageFlagBits, TextureUsageFlags);

enum class SampleCountFlagBits
{
	Count1 = 1 << 0,
	Count2 = 1 << 1,
	Count4 = 1 << 2,
	Count8 = 1 << 3,
	Count16 = 1 << 4,
	Count32 = 1 << 5,
	Count64 = 1 << 6,
};
ENABLE_FLAG_ENUMS(SampleCountFlagBits, SampleCountFlags);

struct TextureCreateInfo
{
	TextureType type;
	MemoryUsage mem_usage;
	Format format;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t mip_levels;
	uint32_t array_layers;
	SampleCountFlagBits sample_count;
	TextureUsageFlags usage_flags;
	TextureLayout initial_layout;

	TextureCreateInfo(TextureType in_type = TextureType::Tex2D,
		MemoryUsage in_mem_usage = MemoryUsage::GpuOnly,
		Format in_format = Format::R8G8B8A8Unorm,
		uint32_t in_width = 0,
		uint32_t in_height = 0,
		uint32_t in_depth = 0,
		uint32_t in_mip_levels = 0,
		uint32_t in_array_layers = 0,
		SampleCountFlagBits in_sample_count = SampleCountFlagBits::Count1,
		TextureUsageFlags in_usage_flags = TextureUsageFlags(),
		TextureLayout in_initial_layout = TextureLayout::Undefined) : type(in_type), mem_usage(in_mem_usage),
		format(in_format), width(in_width), height(in_height), depth(in_depth),
		mip_levels(in_mip_levels), array_layers(in_array_layers), sample_count(in_sample_count),
		usage_flags(in_usage_flags), initial_layout(in_initial_layout) {}
};

struct TextureViewCreateInfo
{
	ResourceHandle texture;
	TextureViewType type;
	Format format;
	TextureSubresourceRange subresource_range;

	TextureViewCreateInfo() : type(TextureViewType::Tex1D),
		format(Format::Undefined) {}

	TextureViewCreateInfo(const ResourceHandle& in_texture,
		TextureViewType in_type, Format in_format, const TextureSubresourceRange& in_subresource_range) :
		texture(in_texture), type(in_type), format(in_format), subresource_range(in_subresource_range) {}
};

/** Command related structures */
enum class CommandPoolType
{
	/** Accept gfx, compute & transfer  */
	Gfx,
	Compute,
	Transfer
};

/** Barrier related structures */
enum class AccessFlagBits
{
	TransferRead = 1 << 0,
	TransferWrite = 1 << 1,
	ShaderRead = 1 << 2,
	ShaderWrite = 1 << 3,
	HostRead = 1 << 4,
	HostWrite = 1 << 5,
	MemoryRead = 1 << 6,
	MemoryWrite = 1 << 7,
	ColorAttachmentRead = 1 << 8,
	ColorAttachmentWrite = 1 << 9,
	DepthStencilAttachmentRead = 1 << 10,
	DepthStencilAttachmentWrite = 1 << 11,
	InputAttachmentRead = 1 << 11,
	UniformRead = 1 << 12,
};
ENABLE_FLAG_ENUMS(AccessFlagBits, AccessFlags);

struct TextureMemoryBarrier
{
	ResourceHandle texture;
	AccessFlags src_access_flags;
	AccessFlags dst_access_flags;
	TextureLayout old_layout;
	TextureLayout new_layout;
	TextureSubresourceRange subresource_range;

	TextureMemoryBarrier(const ResourceHandle& in_texture,
		AccessFlags in_src_access_flags,
		AccessFlags in_dst_access_flags,
		TextureLayout in_old_layout,
		TextureLayout in_new_layout,
		const TextureSubresourceRange& in_subresource_range) :
		texture(in_texture), src_access_flags(in_src_access_flags), dst_access_flags(in_dst_access_flags),
		old_layout(in_old_layout), new_layout(in_new_layout),
		subresource_range(in_subresource_range) {}
};

/** Pipeline related structures */

/**
 * Each stage of a pipeline
 */
enum class PipelineStageFlagBits
{
	TopOfPipe = 1 << 0,
	InputAssembler = 1 << 1,
	VertexShader = 1 << 2,
	TesselationControlShader = 1 << 3,
	TesselationEvaluationShader = 1 << 4,
	GeometryShader = 1 << 5,
	FragmentShader = 1 << 6,
	EarlyFragmentTests = 1 << 7,
	LateFragmentTests = 1 << 8,
	ColorAttachmentOutput = 1 << 9,
	ComputeShader = 1 << 10,
	Transfer = 1 << 11,
	BottomOfPipe = 1 << 12,

	/** Graphics stages */
	AllGraphics = InputAssembler | VertexShader | TesselationControlShader | 
		TesselationEvaluationShader | GeometryShader | FragmentShader | EarlyFragmentTests | LateFragmentTests | ColorAttachmentOutput,
};
ENABLE_FLAG_ENUMS(PipelineStageFlagBits, PipelineStageFlags);

enum class VertexInputRate
{
	Vertex,
	Instance
};

/**
 * Description of a vertex buffer input binding
 */
struct VertexInputBindingDescription
{
	uint32_t binding;
	uint32_t stride;
	VertexInputRate input_rate;

	VertexInputBindingDescription(const uint32_t in_binding,
		const uint32_t in_stride, const VertexInputRate in_input_rate) 
		: binding(in_binding), stride(in_stride), input_rate(in_input_rate) {}

	bool operator==(const VertexInputBindingDescription& in_other) const
	{
		return binding == in_other.binding &&
			stride == in_other.stride &&
			input_rate == in_other.input_rate;
	}
};

/**
 * Description of a single binding (how the backend interpret it)
 */
struct VertexInputAttributeDescription
{
	uint32_t location;
	uint32_t binding;
	Format format;
	uint32_t offset;

	VertexInputAttributeDescription(const uint32_t in_location,
		const uint32_t in_binding, const Format in_format,
		const uint32_t in_offset) : location(in_location),
		binding(in_binding), format(in_format), offset(in_offset) {}

	bool operator==(const VertexInputAttributeDescription& in_other) const
	{
		return location == in_other.location &&
			binding == in_other.binding &&
			format == in_other.format &&
			offset == in_other.offset;
	}
};

struct PipelineVertexInputStateCreateInfo
{
	std::vector<VertexInputBindingDescription> input_binding_descriptions;
	std::vector<VertexInputAttributeDescription> input_attribute_descriptions;

	PipelineVertexInputStateCreateInfo(const std::vector<VertexInputBindingDescription>& in_input_binding_descriptions = {},
		std::vector<VertexInputAttributeDescription> in_input_attribute_descriptions = {})
		: input_binding_descriptions(in_input_binding_descriptions), input_attribute_descriptions(in_input_attribute_descriptions) {}

	bool operator==(const PipelineVertexInputStateCreateInfo& in_other) const
	{
		return input_binding_descriptions == in_other.input_binding_descriptions &&
			input_attribute_descriptions == in_other.input_attribute_descriptions;
	}
};

enum class PolygonMode
{
	Fill,
	Line
};

enum class CullMode
{
	None,
	Front,
	Back,
	FrontAndBack
};

enum class FrontFace
{
	CounterClockwise,
	Clockwise
};

/**
 * Describe the rasterizer state
 */
struct PipelineRasterizationStateCreateInfo
{
	PolygonMode polygon_mode;
	CullMode cull_mode;
	FrontFace front_face;
	bool enable_depth_clamp;
	bool enable_depth_bias;
	float depth_bias_constant_factor;
	float depth_bias_clamp;
	float depth_bias_slope_factor;

	PipelineRasterizationStateCreateInfo(
		const PolygonMode in_polygon_mode = PolygonMode::Fill,
		const CullMode in_cull_mode = CullMode::None, 
		const FrontFace in_front_face = FrontFace::CounterClockwise,
		const bool in_enable_depth_clamp = 0.0f, 
		const bool in_enable_depth_bias = false,
		const float in_depth_bias_constant_factor = 0.0f, 
		const float in_depth_bias_clamp = 0.0f,
		const float in_depth_bias_slope_factor = 0.0f) : polygon_mode(in_polygon_mode),
		cull_mode(in_cull_mode), front_face(in_front_face), enable_depth_clamp(in_enable_depth_clamp),
		enable_depth_bias(in_enable_depth_bias), depth_bias_constant_factor(in_depth_bias_constant_factor),
		depth_bias_clamp(in_depth_bias_clamp), depth_bias_slope_factor(in_depth_bias_slope_factor) {}

	bool operator==(const PipelineRasterizationStateCreateInfo& in_other) const
	{
		return polygon_mode == in_other.polygon_mode &&
			cull_mode == in_other.cull_mode &&
			front_face == in_other.front_face &&
			enable_depth_clamp == in_other.enable_depth_clamp &&
			enable_depth_bias == in_other.enable_depth_bias &&
			depth_bias_constant_factor == in_other.depth_bias_constant_factor &&
			depth_bias_clamp == in_other.depth_bias_clamp &&
			depth_bias_slope_factor == in_other.depth_bias_slope_factor;
	}
};

struct PipelineMultisamplingStateCreateInfo
{
	SampleCountFlagBits samples;

	PipelineMultisamplingStateCreateInfo(const SampleCountFlagBits& in_samples = SampleCountFlagBits::Count1)
		: samples(in_samples) {}

	bool operator==(const PipelineMultisamplingStateCreateInfo& in_other) const
	{
		return samples == in_other.samples;
	}
};

enum class CompareOp
{
	Never,
	Less,
	Equal,
	LessOrEqual,
	Greater,
	NotEqual,
	GreaterOrEqual,
	Always
};

enum class StencilOp
{
	Keep,
	Zero,
	Replace,
	IncrementAndClamp,
	DecrementAndClamp,
	Invert,
	IncrementAndWrap,
	DecrementAndWrap
};

struct StencilOpState
{
	StencilOp fail_op;
	StencilOp pass_op;
	StencilOp depth_fail_op;
	CompareOp compare_op;
	uint32_t compare_mask;
	uint32_t write_mask;
	uint32_t reference;

	StencilOpState(const StencilOp in_fail_op = StencilOp::Zero,
		const StencilOp in_pass_op = StencilOp::Zero,
		const StencilOp in_depth_fail_op = StencilOp::Zero,
		const CompareOp in_compare_op = CompareOp::Never,
		const uint32_t in_compare_mask = 0,
		const uint32_t in_write_mask = 0,
		const uint32_t in_reference = 0) : fail_op(in_fail_op),
		pass_op(in_pass_op), depth_fail_op(in_depth_fail_op),
		compare_op(in_compare_op), compare_mask(in_compare_mask),
		write_mask(in_write_mask), reference(in_reference) {}

	bool operator==(const StencilOpState& in_other) const
	{
		return fail_op == in_other.fail_op &&
			pass_op == in_other.pass_op &&
			depth_fail_op == in_other.depth_fail_op &&
			compare_op == in_other.compare_op &&
			compare_mask == in_other.compare_mask &&
			write_mask == in_other.write_mask &&
			reference == in_other.reference;
	}
};

struct PipelineDepthStencilStateCreateInfo
{
	bool enable_depth_test;
	bool enable_depth_write;
	CompareOp depth_compare_op;
	bool enable_depth_bounds_test;
	bool enable_stencil_test;
	StencilOpState front_face;
	StencilOpState back_face;

	PipelineDepthStencilStateCreateInfo(const bool in_enable_depth_test = false,
		const bool in_enable_depth_write = false,
		const CompareOp in_compare_op = CompareOp::Never,
		const bool in_enable_depth_bounds_test = false,
		const bool in_enable_stencil_test = false,
		const StencilOpState in_front_face = StencilOpState(),
		const StencilOpState in_back_face = StencilOpState()) : enable_depth_test(in_enable_depth_test),
		enable_depth_write(in_enable_depth_write), depth_compare_op(in_compare_op),
		enable_depth_bounds_test(in_enable_depth_bounds_test), enable_stencil_test(in_enable_stencil_test),
		front_face(in_front_face), back_face(in_back_face) {}

	bool operator==(const PipelineDepthStencilStateCreateInfo& in_other) const
	{
		return enable_depth_test == in_other.enable_depth_test &&
			enable_depth_write == in_other.enable_depth_write &&
			depth_compare_op == in_other.depth_compare_op &&
			enable_depth_bounds_test == in_other.enable_depth_bounds_test &&
			enable_stencil_test == in_other.enable_stencil_test &&
			front_face == in_other.front_face &&
			back_face == in_other.back_face;
	}
};

static constexpr size_t max_attachments_per_framebuffer = 8;

enum class BlendFactor
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	DstColor,
	OneMinusDstColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	ConstantColor,
	OneMinusConstantColor,
	ConstantAlpha,
	OneMinusConstantAlpha,
};

enum class BlendOp
{
	Add,
	Substract,
	ReverseSubstract,
	Min,
	Max
};

enum class ColorComponentFlagBits
{
	R = 1 << 0,
	G = 1 << 1,
	B = 1 << 2,
	A = 1 << 3,

	RGBA = R | G | B | A
};
ENABLE_FLAG_ENUMS(ColorComponentFlagBits, ColorComponentFlags);

/**
 * Color blend attachment of a single attachment
 */
struct PipelineColorBlendAttachmentState
{
	bool enable_blend;
	BlendFactor src_color_blend_factor;
	BlendFactor dst_color_blend_factor;
	BlendOp color_blend_op;
	BlendFactor src_alpha_blend_factor;
	BlendFactor dst_alpha_blend_factor;
	BlendOp alpha_blend_op;
	ColorComponentFlags color_write_flags;

	PipelineColorBlendAttachmentState(const bool in_enable_blend = false,
		const BlendFactor in_src_color_blend_factor = BlendFactor::One,
		const BlendFactor in_dst_color_blend_factor = BlendFactor::Zero,
		const BlendOp in_color_blend_op = BlendOp::Add,
		const BlendFactor in_src_alpha_blend_factor = BlendFactor::One,
		const BlendFactor in_dst_alpha_blend_factor = BlendFactor::Zero,
		const BlendOp in_alpha_blend_op = BlendOp::Add,
		const ColorComponentFlags in_color_write_flags = ColorComponentFlagBits::RGBA) :
		enable_blend(in_enable_blend), src_color_blend_factor(in_src_color_blend_factor),
		dst_color_blend_factor(in_dst_color_blend_factor), color_blend_op(in_color_blend_op),
		src_alpha_blend_factor(in_src_alpha_blend_factor), dst_alpha_blend_factor(in_dst_alpha_blend_factor),
		alpha_blend_op(in_alpha_blend_op), color_write_flags(in_color_write_flags) {}

	bool operator==(const PipelineColorBlendAttachmentState& in_other) const
	{
		return enable_blend == in_other.enable_blend &&
			src_color_blend_factor == in_other.src_color_blend_factor &&
			dst_color_blend_factor == in_other.dst_color_blend_factor &&
			color_blend_op == in_other.color_blend_op &&
			src_alpha_blend_factor == in_other.src_alpha_blend_factor &&
			dst_alpha_blend_factor == in_other.dst_alpha_blend_factor &&
			alpha_blend_op == in_other.alpha_blend_op &&
			color_write_flags == in_other.color_write_flags;
	}
};

enum class LogicOp
{
	Clear,
	And,
	AndReverse,
	Copy,
	AndInverted,
	NoOp,
	Xor,
	Or,
	Nor,
	Equivalent,
	Invert,
	OrReverse,
	CopyInverted,
	OrInverted,
	Nand,
	Set
};

struct PipelineColorBlendStateCreationInfo
{
	bool enable_logic_op;
	LogicOp logic_op;
	std::array<PipelineColorBlendAttachmentState, max_attachments_per_framebuffer> attachment_states;

	PipelineColorBlendStateCreationInfo(const bool in_enable_logic_op = false,
		LogicOp in_logic_op = LogicOp::NoOp,
		const std::array<PipelineColorBlendAttachmentState, max_attachments_per_framebuffer>& in_attachment_states = {})
		: enable_logic_op(in_enable_logic_op), logic_op(in_logic_op),
		attachment_states(in_attachment_states) {}

	bool operator==(const PipelineColorBlendStateCreationInfo& in_other) const
	{
		return enable_logic_op == in_other.enable_logic_op &&
			logic_op == in_other.logic_op &&
			attachment_states == in_other.attachment_states;
	}
};

enum class ShaderStageFlagBits
{
	Vertex = 1 << 0,
	TesselationControl = 1 << 1,
	TesselationEvaluation = 1 << 2,
	Geometry = 1 << 3,
	Fragment = 1 << 4,
	Compute = 1 << 5,

	/** RTX */
};
ENABLE_FLAG_ENUMS(ShaderStageFlagBits, ShaderStageFlags);

/**
 * A single shader stage of a pipeline
 */
struct GfxPipelineShaderStage
{
	ShaderStageFlagBits shader_stage;
	ResourceHandle shader;
	const char* entry_point;

	GfxPipelineShaderStage(const ShaderStageFlagBits& in_shader_stage,
		const ResourceHandle& in_shader,
		const char* in_entry_point) : shader_stage(in_shader_stage),
		shader(in_shader), entry_point(in_entry_point) {}

	bool operator==(const GfxPipelineShaderStage& in_other) const
	{
		return shader_stage == in_other.shader_stage &&
			shader == in_other.shader &&
			entry_point == in_other.entry_point;
	}
};

enum class PrimitiveTopology
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	PatchList
};

struct PipelineInputAssemblyStateCreateInfo
{
	PrimitiveTopology primitive_topology;

	PipelineInputAssemblyStateCreateInfo(const PrimitiveTopology in_topology = PrimitiveTopology::TriangleList)
		: primitive_topology(in_topology) {}

	bool operator==(const PipelineInputAssemblyStateCreateInfo& in_other) const
	{
		return primitive_topology == in_other.primitive_topology;
	}
};

struct GfxPipelineCreateInfo
{
	std::vector<GfxPipelineShaderStage> shader_stages;
	PipelineVertexInputStateCreateInfo vertex_input_state;
	PipelineInputAssemblyStateCreateInfo input_assembly_state;
	PipelineRasterizationStateCreateInfo rasterization_state;
	PipelineMultisamplingStateCreateInfo multisampling_state;
	PipelineDepthStencilStateCreateInfo depth_stencil_state;
	PipelineColorBlendStateCreationInfo color_blend_state;
	ResourceHandle pipeline_layout;
	ResourceHandle render_pass;

	/** Subpass where this pipeline will be used */
	uint32_t subpass;

	GfxPipelineCreateInfo(const std::vector<GfxPipelineShaderStage>& in_shader_stages = {},
		const PipelineVertexInputStateCreateInfo& in_vertex_input_state = PipelineVertexInputStateCreateInfo(),
		const PipelineInputAssemblyStateCreateInfo& in_input_assembly_state = PipelineInputAssemblyStateCreateInfo(),
		const PipelineRasterizationStateCreateInfo& in_rasterization_state = PipelineRasterizationStateCreateInfo(),
		const PipelineMultisamplingStateCreateInfo& in_multisampling_state = PipelineMultisamplingStateCreateInfo(),
		const PipelineDepthStencilStateCreateInfo& in_depth_stencil_state = PipelineDepthStencilStateCreateInfo(),
		const PipelineColorBlendStateCreationInfo& in_color_blend_state = PipelineColorBlendStateCreationInfo(),
		const ResourceHandle& in_pipeline_layout = ResourceHandle(),
		const ResourceHandle& in_render_pass = ResourceHandle(),
		const uint32_t& in_subpass = 0) :
		shader_stages(in_shader_stages), vertex_input_state(in_vertex_input_state),
		input_assembly_state(in_input_assembly_state), rasterization_state(in_rasterization_state),
		multisampling_state(in_multisampling_state), depth_stencil_state(in_depth_stencil_state),
		color_blend_state(in_color_blend_state), pipeline_layout(in_pipeline_layout), render_pass(in_render_pass), subpass(in_subpass) {}

	bool operator==(const GfxPipelineCreateInfo& in_create_info) const
	{
		return shader_stages == in_create_info.shader_stages &&
			vertex_input_state == in_create_info.vertex_input_state &&
			input_assembly_state == in_create_info.input_assembly_state &&
			rasterization_state == in_create_info.rasterization_state &&
			multisampling_state == in_create_info.multisampling_state &&
			depth_stencil_state == in_create_info.depth_stencil_state &&
			color_blend_state == in_create_info.color_blend_state &&
			pipeline_layout == in_create_info.pipeline_layout &&
			render_pass == in_create_info.render_pass &&
			subpass == in_create_info.subpass;
	}
};

/** Pipeline layout */

enum class DescriptorType
{
	UniformBuffer,
	Sampler,
	SampledTexture,
	StorageBuffer,
	InputAttachment
};

struct DescriptorSetLayoutBinding
{
	uint32_t binding;
	DescriptorType descriptor_type;
	uint32_t count;
	ShaderStageFlags stage_flags;

	DescriptorSetLayoutBinding(const uint32_t in_binding,
		const DescriptorType in_descriptor_type,
		const uint32_t in_count,
		const ShaderStageFlags in_stage_flags) : binding(in_binding),
		descriptor_type(in_descriptor_type), count(in_count), stage_flags(in_stage_flags) {}
};

/*
 * Layout of a single descriptor set
 */
struct DescriptorSetLayoutCreateInfo
{
	std::vector<DescriptorSetLayoutBinding> bindings;

	DescriptorSetLayoutCreateInfo(const std::vector<DescriptorSetLayoutBinding>& in_bindings = {}) :
		bindings(in_bindings) {}
};

/** 
 * A single push constant range
 */
struct PushConstantRange
{
	ShaderStageFlags stage_flags;
	uint32_t offset;
	uint32_t size;

	PushConstantRange(const ShaderStageFlags& in_stage_flags,
		const uint32_t& in_offset,
		const uint32_t& in_size) : stage_flags(in_stage_flags),
		offset(in_offset), size(in_size) {}
};

struct PipelineLayoutCreateInfo
{
	std::vector<DescriptorSetLayoutCreateInfo> set_layouts;
	std::vector<PushConstantRange> push_constant_ranges;

	PipelineLayoutCreateInfo(const std::vector<DescriptorSetLayoutCreateInfo>& in_set_layouts = {},
		const std::vector<PushConstantRange>& in_ranges = {}) : set_layouts(in_set_layouts), push_constant_ranges(in_ranges) {}
};

enum class PipelineBindPoint
{
	Gfx,
	Compute,
	Raytracing
};

/** Render pass/framebuffer */
enum class AttachmentLoadOp
{
	Load,
	Clear,
	DontCare
};

enum class AttachmentStoreOp
{
	Store,
	DontCare
};

/** Description of an attachment */
struct AttachmentDescription
{
	Format format;
	SampleCountFlagBits samples;
	AttachmentLoadOp load_op;
	AttachmentStoreOp store_op;
	AttachmentLoadOp stencil_load_op;
	AttachmentStoreOp stencil_store_op;
	TextureLayout initial_layout;
	TextureLayout final_layout;

	AttachmentDescription(const Format& in_format,
		const SampleCountFlagBits& in_samples,
		const AttachmentLoadOp& in_load_op,
		const AttachmentStoreOp& in_store_op,
		const AttachmentLoadOp& in_stencil_load_op,
		const AttachmentStoreOp& in_stencil_store_op,
		const TextureLayout& in_initial_layout,
		const TextureLayout& in_final_layout) : format(in_format),
		samples(in_samples), load_op(in_load_op), store_op(in_store_op),
		stencil_load_op(in_stencil_load_op), stencil_store_op(in_stencil_store_op),
		initial_layout(in_initial_layout), final_layout(in_final_layout) {}

	bool operator==(const AttachmentDescription& in_desc) const
	{
		return format == in_desc.format &&
			samples == in_desc.samples &&
			load_op == in_desc.load_op &&
			store_op == in_desc.store_op &&
			stencil_load_op == in_desc.stencil_load_op &&
			stencil_store_op == in_desc.stencil_store_op &&
			initial_layout == in_desc.initial_layout &&
			final_layout == in_desc.final_layout;
	}
};

/**
 * Reference to an attachment
 */
struct AttachmentReference
{
	static constexpr uint32_t unused_attachment = ~0Ui32;

	/** Index of the attachmenet (mirror RenderPassCreateInfo::attachments) */
	uint32_t attachment;

	/** Attachment layout during the subpass */
	TextureLayout layout;

	AttachmentReference() : attachment(unused_attachment), layout(TextureLayout::Undefined) {}

	AttachmentReference(const uint32_t in_attachment,
		const TextureLayout in_layout) : attachment(in_attachment), layout(in_layout) {}

	bool operator==(const AttachmentReference& in_ref) const
	{
		return attachment == in_ref.attachment &&
			layout == in_ref.layout;
	}
};

struct SubpassDescription
{
	std::vector<AttachmentReference> input_attachments;
	std::vector<AttachmentReference> color_attachments;
	std::vector<AttachmentReference> resolve_attachments;
	AttachmentReference depth_stencil_attachment;
	std::vector<uint32_t> preserve_attachments;

	SubpassDescription(const std::vector<AttachmentReference>& in_input_attachments = {},
		const std::vector<AttachmentReference>& in_color_attachments = {},
		const std::vector<AttachmentReference>& in_resolve_attachments = {},
		const AttachmentReference& in_depth_stencil_attachment = AttachmentReference(),
		const std::vector<uint32_t>& in_preserve_attachments = {}) :
		input_attachments(in_input_attachments), color_attachments(in_color_attachments),
		resolve_attachments(in_resolve_attachments), depth_stencil_attachment(in_depth_stencil_attachment),
		preserve_attachments(in_preserve_attachments) {}

	bool operator==(const SubpassDescription& in_desc) const
	{
		return input_attachments == in_desc.input_attachments &&
			color_attachments == in_desc.color_attachments &&
			resolve_attachments == in_desc.resolve_attachments &&
			depth_stencil_attachment == in_desc.depth_stencil_attachment &&
			preserve_attachments == in_desc.preserve_attachments;
	}
};

struct RenderPassCreateInfo
{
	std::vector<AttachmentDescription> attachments;
	std::vector<SubpassDescription> subpasses;

	RenderPassCreateInfo(const std::vector<AttachmentDescription>& in_attachments,
		const std::vector<SubpassDescription>& in_subpasses) : attachments(in_attachments),
		subpasses(in_subpasses) {}

	bool operator==(const RenderPassCreateInfo& in_info) const
	{
		return attachments == in_info.attachments &&
			subpasses == in_info.subpasses;
	}
};

union ClearColorValue
{
	std::array<float, 4> float32;

	ClearColorValue(const std::array<float, 4>& in_float) : float32(in_float) {}
};

struct ClearDepthStencilValue
{
	float depth;
	uint32_t stencil;

	ClearDepthStencilValue(const float in_depth,
		const uint32_t in_stencil) : depth(in_depth), stencil(in_stencil) {}
};

union ClearValue
{
	ClearColorValue color;
	ClearDepthStencilValue depth_stencil;

	ClearValue(const ClearColorValue& in_color) :
		color(in_color) {}

	ClearValue(const ClearDepthStencilValue& in_depth_stencil) :
		depth_stencil(in_depth_stencil) {}
};

/**
 * A framebuffer (containers of texture views)
 */
struct Framebuffer
{
	std::array<ResourceHandle, max_attachments_per_framebuffer> color_attachments;
	std::array<ResourceHandle, max_attachments_per_framebuffer> depth_attachments;
	uint32_t width;
	uint32_t height;
	uint32_t layers;

	Framebuffer() : width(0), height(0), layers(1) {}

	bool operator==(const Framebuffer& other) const
	{
		return color_attachments == other.color_attachments &&
			depth_attachments == other.depth_attachments &&
			width == other.width &&
			height == other.height;
	}
};

/** Shaders structures */
struct ShaderCreateInfo
{
	std::span<uint32_t> bytecode;

	explicit ShaderCreateInfo(const std::span<uint32_t>& in_bytecode) : bytecode(in_bytecode) {}
};

struct DescriptorBufferInfo
{
	static constexpr uint64_t whole_range = -1;

	ResourceHandle buffer;
	uint64_t offset;
	uint64_t range;

	DescriptorBufferInfo() : offset(0), range(0) {}
	DescriptorBufferInfo(const ResourceHandle& in_buffer,
		const uint64_t in_offset = 0,
		const uint64_t in_range = whole_range) : buffer(in_buffer), offset(in_offset),
		range(in_range) {}

	bool operator==(const DescriptorBufferInfo& in_other) const
	{
		return buffer == in_other.buffer &&
			offset == in_other.offset &&
			range == in_other.range;
	}
};

struct DescriptorTextureInfo
{
	ResourceHandle handle;
	TextureLayout layout;
	
	DescriptorTextureInfo() {}
	DescriptorTextureInfo(const ResourceHandle& in_handle,
		const TextureLayout in_layout = gfx::TextureLayout::Undefined) : handle(in_handle), layout(in_layout) {}

	bool operator==(const DescriptorTextureInfo& in_other) const
	{
		return handle == in_other.handle &&
			layout == in_other.layout;
	}
};

/** 
 * Binding of a descriptor set
 */
struct Descriptor
{
	DescriptorType type;
	uint32_t dst_binding;
	std::variant<DescriptorBufferInfo, DescriptorTextureInfo> info;
	
	Descriptor() : type(DescriptorType::UniformBuffer),
		dst_binding(-1), info(DescriptorBufferInfo()) {}

	Descriptor(const DescriptorType in_type,
		const uint32_t in_dst_binding,
		const DescriptorBufferInfo& in_buffer) : type(in_type),
		dst_binding(in_dst_binding), info(in_buffer) {}

	Descriptor(const DescriptorType in_type,
		const uint32_t in_dst_binding,
		const DescriptorTextureInfo& in_texture) : type(in_type),
		dst_binding(in_dst_binding), info(in_texture) {}

	bool operator==(const Descriptor& in_other) const
	{
		return type == in_other.type &&
			dst_binding == in_other.dst_binding &&
			info == in_other.info;
	}
};

/** Descriptor set */
struct DescriptorSetCreateInfo
{
	ResourceHandle pipeline_layout;
	std::vector<Descriptor> descriptors;

	explicit DescriptorSetCreateInfo(const ResourceHandle& in_pipeline_layout,
		const std::vector<Descriptor>& in_descriptors)
		: pipeline_layout(in_pipeline_layout), descriptors(in_descriptors) {}
};

enum class IndexType
{
	Uint16,
	Uint32
};

struct Viewport
{
	float x;
	float y;
	float width;
	float height;
	float min_depth;
	float max_depth;

	Viewport(const float in_x = 0.f,
		const float in_y = 0.f,
		const float in_width = 0.f,
		const float in_height = 0.f,
		const float in_min_depth = 0.0f,
		const float in_max_depth = 1.0f) : x(in_x), y(in_y),
		width(in_width), height(in_height), min_depth(in_min_depth), max_depth(in_max_depth) {}
};

/** Transfers structure */
struct BufferCopyRegion
{
    uint64_t src_offset;
    uint64_t dst_offset;
    uint64_t size;
    
    BufferCopyRegion(const uint64_t& in_src_offset,
		const uint64_t& in_dst_offset,
		const uint64_t& in_size) : src_offset(in_src_offset),
		dst_offset(in_dst_offset),
		size(in_size) {}
};

struct BufferTextureCopyRegion
{
	/** Offset to where to start copying */
	uint64_t buffer_offset;
	TextureSubresourceLayers texture_subresource;
	gfx::Offset3D texture_offset;
	gfx::Extent3D texture_extent;

	BufferTextureCopyRegion(const uint64_t& in_buffer_offset,
		const TextureSubresourceLayers& in_texture_subresource,
		const gfx::Offset3D& in_texture_offset,
		const gfx::Extent3D& in_texture_extent) : buffer_offset(in_buffer_offset),
		texture_subresource(in_texture_subresource), texture_offset(in_texture_offset),
		texture_extent(in_texture_extent) {}
};

struct TextureCopyRegion
{
	TextureSubresourceLayers src_subresource;
	gfx::Offset3D src_offset;
	TextureSubresourceLayers dst_subresource;
	gfx::Offset3D dst_offset;
	gfx::Extent3D extent;

	TextureCopyRegion(const TextureSubresourceLayers& in_src_subresource,
		const gfx::Offset3D& in_src_offset,
		const TextureSubresourceLayers& in_dst_subresource,
		const gfx::Offset3D& in_dst_offset,
		const gfx::Extent3D& in_extent) : src_subresource(in_src_subresource),
		src_offset(in_src_offset), dst_subresource(in_dst_subresource),
		dst_offset(in_dst_offset), extent(in_extent) {}
};

struct TextureBlitRegion
{
	TextureSubresourceLayers src_subresource;
	std::array<Offset3D, 2> src_offsets;
	TextureSubresourceLayers dst_subresource;
	std::array<Offset3D, 2> dst_offsets;

	TextureBlitRegion(const TextureSubresourceLayers& in_src_subresource,
		const std::array<gfx::Offset3D, 2>& in_src_offsets,
		const TextureSubresourceLayers& in_dst_subresource,
		const std::array<gfx::Offset3D, 2>& in_dst_offsets) : src_subresource(in_src_subresource),
		src_offsets(in_src_offsets), dst_subresource(in_dst_subresource),
		dst_offsets(in_dst_offsets) {}
};

/** Sampler structures */
enum class Filter
{
	Nearest,
	Linear,
};

enum class SamplerAddressMode
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
};

struct SamplerCreateInfo
{
	Filter min_filter;
	Filter mag_filter;
	Filter mip_map_mode;
	SamplerAddressMode address_mode_u;
	SamplerAddressMode address_mode_v;
	SamplerAddressMode address_mode_w;
	float mip_lod_bias;
	bool enable_compare_op;
	CompareOp compare_op;
	bool enable_anisotropy;
	float max_anisotropy;
	float min_lod;
	float max_lod;

	SamplerCreateInfo(
		const Filter& in_min_filter = Filter::Linear,
		const Filter& in_mag_filter = Filter::Linear,
		const Filter& in_mip_map_mode = Filter::Linear,
		const SamplerAddressMode& in_address_mode_u = SamplerAddressMode::Repeat,
		const SamplerAddressMode& in_address_mode_v = SamplerAddressMode::Repeat,
		const SamplerAddressMode& in_address_mode_w = SamplerAddressMode::Repeat,
		const float in_mip_lod_bias = 0.f,
		const bool in_enable_compare_op = false,
		const CompareOp& in_compare_op = CompareOp::Never,
		const bool in_enable_aniostropy = false,
		const float in_max_anisotropy = 0.f,
		const float in_min_lod = 0.f,
		const float in_max_lod = 0.f) :
		min_filter(in_min_filter), mag_filter(in_mag_filter),
		mip_map_mode(in_mip_map_mode), address_mode_u(in_address_mode_u),
		address_mode_v(in_address_mode_v), address_mode_w(in_address_mode_w),
		mip_lod_bias(in_mip_lod_bias), enable_compare_op(in_enable_compare_op), compare_op(in_compare_op),
		enable_anisotropy(in_enable_aniostropy), max_anisotropy(in_max_anisotropy),
		min_lod(in_min_lod), max_lod(in_max_lod) {}
};

enum class BackendFeatureFlagBits
{
	/**
	 * Command pool trimming allows the pool to reclaim memory from allocated command lists that are no longer in use
	 */
	CommandPoolTrimming = 1 << 0,
};
ENABLE_FLAG_ENUMS(BackendFeatureFlagBits, BackendFeatureFlags)

enum class Result
{
	Success = 0,

	ErrorOutOfDeviceMemory = -1,
	ErrorOutOfHostMemory = -2,
	ErrorInitializationFailed = -3,
	ErrorUnknown = -4,
};

/**
 * A backend
 * Abstract away graphics API details to provide a consistant API to use for higher-level APIs
 */
class Backend
{
public:
	Backend(const BackendInfo* in_backend_info);
	virtual ~Backend();

	static Backend& get();
	
	/**
	 * Get shader format for the specified shader model
	 */
	ShaderFormat get_shader_format(const BackendShaderModel in_shader_model) const;
	
	/**
	 * Inform the backend that a new frame has started
	 */
	virtual void new_frame() = 0;

	/** 
	 * Wait for device 
	 */
	virtual void device_wait_idle() = 0;

	/**
	 * Get supported features by this backend
	 */
	virtual BackendFeatureFlags get_features() const = 0;
	
	ZE_FORCEINLINE bool support_feature(BackendFeatureFlagBits feature) const
	{
		return (get_features() & feature) == feature;
	}

	/**
	 * Create a new buffer
	 * \return Handle to the newly created buffer
	 */
	virtual std::pair<Result, ResourceHandle> buffer_create(const BufferCreateInfo& in_create_info) = 0;

	/**
	 * Create a new swap chain
	 */
	virtual std::pair<Result, ResourceHandle> swapchain_create(const SwapChainCreateInfo& in_create_info) = 0;

	/*
	 * Create a texture
	 */
	virtual std::pair<Result, ResourceHandle> texture_create(const TextureCreateInfo& in_create_info) = 0;

	/**
	 * Create a new texture view
	 */
	virtual std::pair<Result, ResourceHandle> texture_view_create(const TextureViewCreateInfo& in_create_info) = 0;

	/**
	 * Create a render pass
	 */
	virtual std::pair<Result, ResourceHandle> render_pass_create(const RenderPassCreateInfo& in_create_info) = 0;

	/**
	 * Create a shader
	 */
	virtual std::pair<Result, ResourceHandle> shader_create(const ShaderCreateInfo& in_create_info) = 0;

	/**
	 * Create a gfx pipeline
	 */
	virtual std::pair<Result, ResourceHandle> gfx_pipeline_create(const GfxPipelineCreateInfo& in_create_info) = 0;

	/**
	 * Create a pipeline layout
	 */
	virtual std::pair<Result, ResourceHandle> pipeline_layout_create(const PipelineLayoutCreateInfo& in_create_info) = 0;

	/** 
	 * Create a sampler
	 */
	virtual std::pair<Result, ResourceHandle> sampler_create(const SamplerCreateInfo& in_create_info) = 0;

	/**
	 * Create a fence
	 */
	virtual std::pair<Result, ResourceHandle> fence_create(const bool in_is_signaled = false) = 0;

	/** Destroy functions */
	virtual void buffer_destroy(const ResourceHandle& in_handle) = 0;
	virtual void texture_destroy(const ResourceHandle& in_handle) = 0;
	virtual void texture_view_destroy(const ResourceHandle& in_handle) = 0;
	virtual void swapchain_destroy(const ResourceHandle& in_handle) = 0;
	virtual void fence_destroy(const ResourceHandle& in_handle) = 0;
	virtual void semaphore_destroy(const ResourceHandle& in_handle) = 0;
	virtual void command_pool_destroy(const ResourceHandle& in_handle) = 0;
	virtual void shader_destroy(const ResourceHandle& in_handle) = 0;
	virtual void render_pass_destroy(const ResourceHandle& in_handle) = 0;
	virtual void pipeline_destroy(const ResourceHandle& in_handle) = 0;
	virtual void pipeline_layout_destroy(const ResourceHandle& in_handle) = 0;
	virtual void sampler_destroy(const ResourceHandle& in_handle) = 0;

	/** PIPELINE LAYOUT RELATED FUNCTIONS */

	/**
	 * Allocate a descriptor set from a pipeline layout
	 * \remark Backend may use recycled descriptor sets or may use pools and therefore not perform any heap allocation.
	 * \return Allocated descriptor set
	 */
	virtual ResourceHandle pipeline_layout_allocate_descriptor_set(const ResourceHandle& in_pipeline_layout,
		const uint32_t in_set,
		const std::vector<Descriptor>& descriptors) = 0;

	/** BUFFER RELATED FUNCTIONS */
	virtual std::pair<Result, void*> buffer_map(const ResourceHandle& in_buffer) = 0;
	virtual void buffer_unmap(const ResourceHandle& in_buffer) = 0;

	/** SWAP CHAIN RELATED FUNCTIONS */

	/**
	 * Acquire swapchain image, must be called before rendering to a swapchain image in some backends
	 * \return false if swapchain need to be recreated
	 */
	virtual bool swapchain_acquire_image(const ResourceHandle& in_swapchain) = 0;

	/**
	 * Resize the swapchain  
	 */
	virtual void swapchain_resize(const ResourceHandle& in_swapchain, const uint32_t in_new_width,
		const uint32_t in_new_height) = 0;

	/**
	 * Get the backbuffer texture view of the specified swapchain
	 * \return The backbuffer
	 */
	virtual ResourceHandle swapchain_get_backbuffer(const ResourceHandle& in_swapchain) = 0;

	/**
	 * Get the current backbuffer texture of the specified swapchain
	 */
	virtual ResourceHandle swapchain_get_backbuffer_texture(const ResourceHandle& in_swapchain) = 0;

	virtual uint32_t swapchain_get_backbuffer_index(const ResourceHandle& in_swapchain) = 0;
	virtual std::vector<ResourceHandle> swapchain_get_backbuffer_textures(const ResourceHandle& in_swapchain) = 0;
	virtual std::vector<ResourceHandle> swapchain_get_backbuffer_texture_views(const ResourceHandle& in_swapchain) = 0;
	virtual uint32_t swapchain_get_textures_count(const ResourceHandle& in_swapchain) = 0;

	/**
	 * Present a swapchain
	 */
	virtual void swapchain_present(const ResourceHandle& in_swapchain,
		const std::vector<ResourceHandle>& in_wait_semaphores = {}) = 0;

	/** COMMAND RELATED FUNCTIONS */

	/**
	 * Create a command pool
	 */
	virtual ResourceHandle command_pool_create() = 0;
	
	/**
	 * Reset a command pool
	 */
	virtual void command_pool_reset(const ResourceHandle& in_pool) = 0;

	/** 
	 * Allocate a set of command lists from a command pool
	 */
	virtual std::vector<ResourceHandle> command_pool_allocate(const ResourceHandle& in_pool, const size_t in_count) = 0;
	
	/**
	 * Trim a command pool, allowing the pool to reclaim some unused memory
	 * \remark Needs CommandPoolTrimming backend feature !
	 */
	virtual void command_pool_trim(const ResourceHandle& in_pool) = 0;

	/**
	 * Free a set of command lists's memory back to the pool
	 */
	virtual void command_pool_free(const ResourceHandle& in_pool, const std::vector<ResourceHandle>& in_lists) = 0;

	/** 
	 * Begin a command list
	 */
	virtual Result command_list_begin(const ResourceHandle& in_command_list) = 0;

	/**
	 * End a command list
	 */
	virtual Result command_list_end(const ResourceHandle& in_command_list) = 0;

	/** COMMANDS */

	/**
	 * A memory barrier that inserts a dependancy on a resource
	 */
	virtual void cmd_pipeline_barrier(const ResourceHandle& in_cmd_list,
		const PipelineStageFlags& in_src_flags,
		const PipelineStageFlags& in_dst_flags,
		const std::vector<TextureMemoryBarrier>& in_texture_memory_barriers) = 0;

	/** 
	 * Begin a render pass
	 */
	virtual void cmd_begin_render_pass(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_render_pass,
		const Framebuffer& in_framebuffer,
		const maths::Rect2D& in_render_area,
		const std::vector<ClearValue>& in_clear_values) = 0;

	/**
	 * End a render pass
	 */
	virtual void cmd_end_render_pass(const ResourceHandle& in_cmd_list) = 0;

	/**
	 * Bind a pipeline
	 */
	virtual void cmd_bind_pipeline(const ResourceHandle& in_cmd_list,
		const PipelineBindPoint& in_bind_point,
		const ResourceHandle& in_pipeline) = 0;

	/**
	 * Bind vertex buffers
	 */
	virtual void cmd_bind_vertex_buffers(const ResourceHandle& in_cmd_list,
		const uint32_t in_first_binding, 
		const std::vector<ResourceHandle>& in_buffers,
		const std::vector<uint64_t>& in_offsets = {}) = 0;

	/**
	 * Bind an index buffer
	 */
	virtual void cmd_bind_index_buffer(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_buffer,
		const uint64_t in_offset = 0,
		const IndexType in_type = IndexType::Uint32) = 0;

	/** 
	 * Draw a primitive
	 */
	virtual void cmd_draw(const ResourceHandle& in_cmd_list,
		const uint32_t in_vertex_count,
		const uint32_t in_instance_count,
		const uint32_t in_first_vertex,
		const uint32_t in_first_instance) = 0;

	/**
	 * Draw a indexed primitive
	 */
	virtual void cmd_draw_indexed(const ResourceHandle& in_cmd_list,
		const uint32_t in_index_count,
		const uint32_t in_instance_count,
		const uint32_t in_first_index,
		const int32_t in_vertex_offset,
		const uint32_t in_first_instance) = 0;

	/**
	 * Set bound viewports for current pipeline
	 */
	virtual void cmd_set_viewport(const ResourceHandle& in_cmd_list,
		uint32_t in_first_viewport,
		const std::vector<Viewport>& in_viewports) = 0;

	/**
	 * Set bound scissors for current pipeline
	 */
	virtual void cmd_set_scissor(const ResourceHandle& in_cmd_list,
		uint32_t in_first_scissor,
		const std::vector<maths::Rect2D>& in_scissors) = 0;

	/** 
	 * Bind descriptor sets
	 */
	virtual void cmd_bind_descriptor_sets(const ResourceHandle& in_cmd_list,
		const PipelineBindPoint in_bind_point,
		const ResourceHandle& in_pipeline_layout,
		const uint32_t& in_first_set,
		const std::vector<ResourceHandle>& in_descriptor_sets) = 0;


	/**
	 * Update push constants
	 */
	virtual void cmd_push_constants(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_pipeline_layout,
		const ShaderStageFlags in_shader_stage_flags,
		const uint32_t in_offset,
		const uint32_t in_size,
		const void* in_values) = 0;

	/** Transfer related commands */

	/**
	 * Copy a buffer to another buffer
	 */
	virtual void cmd_copy_buffer(const ResourceHandle& in_cmd_list,
        const ResourceHandle& in_src_buffer,
        const ResourceHandle& in_dst_buffer,
        const std::vector<BufferCopyRegion>& in_regions) = 0;
	/**
	 * Copy a buffer to a texture
	 */
	virtual void cmd_copy_buffer_to_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_buffer,
		const ResourceHandle& in_dst_texture,
		const TextureLayout& in_dst_layout,
		const std::vector<BufferTextureCopyRegion>& in_regions) = 0;

	/**
	 * Copy textures
	 */
	virtual void cmd_copy_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_texture,
		const TextureLayout in_dst_layout,
		const std::vector<TextureCopyRegion>& in_regions) = 0;

	/**
	 * Copy texture to buffer
	 */
	virtual void cmd_copy_texture_to_buffer(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_buffer,
		const std::vector<BufferTextureCopyRegion>& in_regions) = 0;

	/**
	 * Blit a texture
	 */
	virtual void cmd_blit_texture(const ResourceHandle& in_cmd_list,
		const ResourceHandle& in_src_texture,
		const TextureLayout in_src_layout,
		const ResourceHandle& in_dst_texture,
		const TextureLayout in_dst_layout,
		const std::vector<TextureBlitRegion>& in_regions,
		const Filter& in_filter) = 0;

	/** QUEUES RELATED FUNCTIONS */

	/**
	 * Execute a command list
	 * \param in_queue Target queue
	 * \param in_command_list List to execute
	 * \parma in_fence Fence to synchronize CPU
	 * \param in_wait_semaphores Semaphore to wait for before executing the command list
	 * \param in_wait_stage_flags Which pipeline stage flags to wait
	 * \param in_signal_semaphores Semaphore to signal after executing the command list
	 * \remark Try to limit usage of queue_execute, batch command lists together if possible
	 */
	virtual void queue_execute(const ResourceHandle& in_queue, 
		const std::vector<ResourceHandle>& in_command_lists,
		const ResourceHandle& in_fence = ResourceHandle(),
		const std::vector<ResourceHandle>& in_wait_semaphores = {},
		const std::vector<PipelineStageFlags>& in_wait_stage_flags = {},
		const std::vector<ResourceHandle>& in_signal_semaphores = {}) = 0;

	/** SYNC PRIMITIVES */
	/**
	 * Wait for multiple fences
	 */
	virtual void fence_wait_for(const std::vector<ResourceHandle>& in_fences,
		const bool in_wait_all = true, const uint64_t in_timeout = std::numeric_limits<uint64_t>::max()) = 0;

	/**
	 * Reset multiple fences
	 */
	virtual void fence_reset(const std::vector<ResourceHandle>& in_fences) = 0;

	/**
	 * Create a semaphore
	 */
	virtual std::pair<Result, ResourceHandle> semaphore_create() = 0;

	/** Getters */

	/**
	 * Get graphics queue
	 * Every backends MUST provide a gfx queue
	 * \return Gfx queue
	 */
	virtual ResourceHandle get_gfx_queue() const = 0;
private:
	const BackendInfo* backend_info;
};
}

namespace std
{
	inline std::string to_string(const ze::gfx::Result& in_result)
	{
		switch(in_result)
		{
		case ze::gfx::Result::Success:
			return "Success";
		case ze::gfx::Result::ErrorUnknown:
			return "Unknown error";
		case ze::gfx::Result::ErrorInitializationFailed:
			return "Object initialization failed";
		case ze::gfx::Result::ErrorOutOfDeviceMemory:
			return "Out of video memory";
		case ze::gfx::Result::ErrorOutOfHostMemory:
			return "Out of memory";
		}
	}

	template<> struct hash<ze::gfx::ResourceHandle>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::ResourceHandle& in_handle) const
		{
			return in_handle.handle;
		}
	};

	template<> struct hash<ze::gfx::BufferCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::BufferCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_create_info.size);
			ze::hash_combine(hash, in_create_info.usage);
			ze::hash_combine(hash, in_create_info.mem_usage);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::SwapChainCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::SwapChainCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_create_info.window_handle);
			ze::hash_combine(hash, in_create_info.width);
			ze::hash_combine(hash, in_create_info.height);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::TextureSubresourceRange>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::TextureSubresourceRange& in_create_info) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_create_info.aspect_flags);
			ze::hash_combine(hash, in_create_info.base_mip_level);
			ze::hash_combine(hash, in_create_info.level_count);
			ze::hash_combine(hash, in_create_info.base_array_layer);
			ze::hash_combine(hash, in_create_info.layer_count);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::TextureCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::TextureCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_create_info.type);
			ze::hash_combine(hash, in_create_info.mem_usage);
			ze::hash_combine(hash, in_create_info.format);
			ze::hash_combine(hash, in_create_info.width);
			ze::hash_combine(hash, in_create_info.height);
			ze::hash_combine(hash, in_create_info.depth);
			ze::hash_combine(hash, in_create_info.mip_levels);
			ze::hash_combine(hash, in_create_info.array_layers);
			ze::hash_combine(hash, in_create_info.sample_count);
			ze::hash_combine(hash, in_create_info.usage_flags);
			ze::hash_combine(hash, in_create_info.initial_layout);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::TextureViewCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::TextureViewCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_create_info.type);
			ze::hash_combine(hash, in_create_info.texture);
			ze::hash_combine(hash, in_create_info.format);
			ze::hash_combine(hash, in_create_info.subresource_range);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::ShaderCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::ShaderCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_create_info.bytecode.data());
			return hash;
		}
	};

	template<> struct hash<ze::gfx::AttachmentDescription>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::AttachmentDescription& in_description) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_description.format);
			ze::hash_combine(hash, in_description.samples);
			ze::hash_combine(hash, in_description.load_op);
			ze::hash_combine(hash, in_description.store_op);
			ze::hash_combine(hash, in_description.stencil_load_op);
			ze::hash_combine(hash, in_description.stencil_store_op);
			ze::hash_combine(hash, in_description.initial_layout);
			ze::hash_combine(hash, in_description.final_layout);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::AttachmentReference>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::AttachmentReference& in_ref) const
		{
			uint64_t hash = 0;
			ze::hash_combine(hash, in_ref.attachment);
			ze::hash_combine(hash, in_ref.layout);
			return hash;
		}
	};

	template<> struct hash<ze::gfx::SubpassDescription>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::SubpassDescription& in_subpass) const
		{
			uint64_t hash = 0;
			for(const auto& attachment : in_subpass.input_attachments)
				ze::hash_combine(hash, attachment);

			for(const auto& attachment : in_subpass.color_attachments)
				ze::hash_combine(hash, attachment);

			for(const auto& attachment : in_subpass.resolve_attachments)
				ze::hash_combine(hash, attachment);

			ze::hash_combine(hash, in_subpass.depth_stencil_attachment);

			for(const auto& attachment : in_subpass.resolve_attachments)
				ze::hash_combine(hash, attachment);


			return hash;
		}
	};

	template<> struct hash<ze::gfx::RenderPassCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::RenderPassCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			for(const auto& attachment : in_create_info.attachments)
				ze::hash_combine(hash, attachment);

			for(const auto& subpass : in_create_info.subpasses)
				ze::hash_combine(hash, subpass);
			return hash;
		}
	};
	
	template<> struct hash<ze::gfx::Framebuffer>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::Framebuffer& in_framebuffer) const
		{
			uint64_t hash = 0;

			for(const auto& attachment : in_framebuffer.color_attachments)
				ze::hash_combine(hash, attachment);

			for(const auto& attachment : in_framebuffer.depth_attachments)
				ze::hash_combine(hash, attachment);

			ze::hash_combine(hash, in_framebuffer.width);
			ze::hash_combine(hash, in_framebuffer.height);
			ze::hash_combine(hash, in_framebuffer.layers);
			return hash;
		}
	};
	
	template<> struct hash<ze::gfx::GfxPipelineShaderStage>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::GfxPipelineShaderStage& in_stage) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_stage.shader_stage);
			ze::hash_combine(hash, in_stage.shader);
			ze::hash_combine(hash, in_stage.entry_point);

			return hash;
		}
	};	

	template<> struct hash<ze::gfx::VertexInputBindingDescription>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::VertexInputBindingDescription& in_binding) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_binding.binding);
			ze::hash_combine(hash, in_binding.input_rate);
			ze::hash_combine(hash, in_binding.stride);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::VertexInputAttributeDescription>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::VertexInputAttributeDescription& in_attribute) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_attribute.binding);
			ze::hash_combine(hash, in_attribute.location);
			ze::hash_combine(hash, in_attribute.format);
			ze::hash_combine(hash, in_attribute.offset);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::PipelineVertexInputStateCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PipelineVertexInputStateCreateInfo& in_state) const
		{
			uint64_t hash = 0;

			for(const auto& binding : in_state.input_binding_descriptions)
				ze::hash_combine(hash, binding);
			
			for(const auto& attribute : in_state.input_attribute_descriptions)
				ze::hash_combine(hash, attribute);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::PipelineInputAssemblyStateCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PipelineInputAssemblyStateCreateInfo& in_state) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_state.primitive_topology);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::PipelineMultisamplingStateCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PipelineMultisamplingStateCreateInfo& in_state) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_state.samples);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::PipelineRasterizationStateCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PipelineRasterizationStateCreateInfo& in_state) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_state.enable_depth_clamp);
			ze::hash_combine(hash, in_state.polygon_mode);
			ze::hash_combine(hash, in_state.cull_mode);
			ze::hash_combine(hash, in_state.enable_depth_bias);
			ze::hash_combine(hash, in_state.depth_bias_constant_factor);
			ze::hash_combine(hash, in_state.depth_bias_slope_factor);
			ze::hash_combine(hash, in_state.depth_bias_clamp);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::StencilOpState>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::StencilOpState& in_state) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_state.pass_op);
			ze::hash_combine(hash, in_state.fail_op);
			ze::hash_combine(hash, in_state.depth_fail_op);
			ze::hash_combine(hash, in_state.compare_op);
			ze::hash_combine(hash, in_state.compare_mask);
			ze::hash_combine(hash, in_state.write_mask);
			ze::hash_combine(hash, in_state.reference);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::PipelineDepthStencilStateCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PipelineDepthStencilStateCreateInfo& in_state) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_state.enable_depth_test);
			ze::hash_combine(hash, in_state.enable_depth_write);
			ze::hash_combine(hash, in_state.depth_compare_op);
			ze::hash_combine(hash, in_state.enable_depth_bounds_test);
			ze::hash_combine(hash, in_state.front_face);
			ze::hash_combine(hash, in_state.back_face);
			
			return hash;
		}
	};

	template<> struct hash<ze::gfx::GfxPipelineCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::GfxPipelineCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;

			for(const auto& stage : in_create_info.shader_stages)
				ze::hash_combine(hash, stage);
			ze::hash_combine(hash, in_create_info.vertex_input_state);
			ze::hash_combine(hash, in_create_info.input_assembly_state);
			ze::hash_combine(hash, in_create_info.multisampling_state);
			ze::hash_combine(hash, in_create_info.rasterization_state);
			ze::hash_combine(hash, in_create_info.depth_stencil_state);
			ze::hash_combine(hash, in_create_info.pipeline_layout);
			ze::hash_combine(hash, in_create_info.render_pass);
			ze::hash_combine(hash, in_create_info.subpass);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::DescriptorBufferInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::DescriptorBufferInfo& in_info) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_info.buffer);
			ze::hash_combine(hash, in_info.offset);
			ze::hash_combine(hash, in_info.range);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::DescriptorTextureInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::DescriptorTextureInfo& in_info) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_info.handle);
			ze::hash_combine(hash, in_info.layout);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::Descriptor>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::Descriptor& in_descriptor) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_descriptor.type);
			ze::hash_combine(hash, in_descriptor.dst_binding);
			ze::hash_combine(hash, in_descriptor.info);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::DescriptorSetLayoutBinding>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::DescriptorSetLayoutBinding& in_binding) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_binding.binding);
			ze::hash_combine(hash, in_binding.count);
			ze::hash_combine(hash, in_binding.descriptor_type);
			ze::hash_combine(hash, in_binding.stage_flags);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::DescriptorSetLayoutCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::DescriptorSetLayoutCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;

			for(const auto& binding : in_create_info.bindings)
				ze::hash_combine(hash, binding);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::PushConstantRange>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PushConstantRange& in_range) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_range.size);
			ze::hash_combine(hash, in_range.offset);
			ze::hash_combine(hash, in_range.stage_flags);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::PipelineLayoutCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::PipelineLayoutCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;

			for(const auto& layout : in_create_info.set_layouts)
				ze::hash_combine(hash, layout);
			
			for(const auto& range : in_create_info.push_constant_ranges)
				ze::hash_combine(hash, range);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::DescriptorSetCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::DescriptorSetCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;
			
			ze::hash_combine(hash, in_create_info.pipeline_layout);

			return hash;
		}
	};

	template<> struct hash<ze::gfx::SamplerCreateInfo>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::gfx::SamplerCreateInfo& in_create_info) const
		{
			uint64_t hash = 0;

			ze::hash_combine(hash, in_create_info.min_filter);
			ze::hash_combine(hash, in_create_info.mag_filter);
			ze::hash_combine(hash, in_create_info.mip_map_mode);
			ze::hash_combine(hash, in_create_info.address_mode_u);
			ze::hash_combine(hash, in_create_info.address_mode_v);
			ze::hash_combine(hash, in_create_info.address_mode_w);
			ze::hash_combine(hash, in_create_info.enable_compare_op);
			ze::hash_combine(hash, in_create_info.compare_op);
			ze::hash_combine(hash, in_create_info.enable_anisotropy);
			ze::hash_combine(hash, in_create_info.max_anisotropy);
			ze::hash_combine(hash, in_create_info.mip_lod_bias);
			ze::hash_combine(hash, in_create_info.min_filter);
			ze::hash_combine(hash, in_create_info.max_lod);

			return hash;
		}
	};
}