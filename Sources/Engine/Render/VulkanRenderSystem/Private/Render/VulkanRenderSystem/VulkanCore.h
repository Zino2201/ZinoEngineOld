#pragma once

#include "EngineCore.h"
#include <optional>
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/RenderPass.h"
#include "Shader/ShaderCore.h"

namespace ZE
{
	enum class EShaderStage;
	enum class ERSBufferUsageFlagBits;
	enum class ERSTextureType;
	enum class ERSTextureUsageFlagBits;
	enum class ERSFilter;
	enum class ERSSamplerAddressMode;
	enum class EVertexInputRate;
	enum class EPolygonMode;
	enum class ECullMode;
	enum class EFrontFace;
	enum class EBlendFactor;
	enum class EBlendOp;
	enum class EStencilOp;
	enum class ERSRenderPassAttachmentLoadOp;
	enum class ERSRenderPassAttachmentStoreOp;
	enum class ERSRenderPassAttachmentLayout;
}

/** OOF */
using namespace ZE;
using namespace ZE::Math;

VULKANRENDERSYSTEM_API extern class CVulkanRenderSystem* GVulkanRenderSystem;
VULKANRENDERSYSTEM_API extern class CVulkanRenderSystemContext* GRenderSystemContext;

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT must
#define VMA_ASSERT(expr) must(expr)

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#ifdef _WIN32
#include <vulkan/vulkan_win32.h>
#endif

/** Constants */
const std::vector<const char*> GVulkanValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool GVulkanEnableValidationLayers = false;
#else
constexpr bool GVulkanEnableValidationLayers = true;
#endif

/** Required device extensions */
const std::vector<const char*> GVulkanRequiredDeviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

/** Max frames in flight */
static constexpr uint32_t GMaxFramesInFlight = 2;

/** Max descriptor sets count */
static constexpr uint8_t GMaxDescriptorSets = 4;

/** Max bindings per set */
static constexpr uint8_t GMaxBindingsPerSet = 16;

class CVulkanDevice;

/**
 * Base class for vulkan device resources
 */
class CVulkanDeviceResource
{
public:
	CVulkanDeviceResource(CVulkanDevice& InDevice) : Device(InDevice) {}
    virtual ~CVulkanDeviceResource() = default;
protected:
    CVulkanDevice& Device;
};

/**
 * A staging buffer (to be used for vulkan render system only)
 * Always host visible
 */
class CVulkanInternalStagingBuffer : public CVulkanDeviceResource,
	public boost::intrusive_ref_counter<CVulkanInternalStagingBuffer,
	        boost::thread_unsafe_counter>
{
public:
    CVulkanInternalStagingBuffer(CVulkanDevice& InDevice,
        uint64_t InSize,
        vk::BufferUsageFlags InUsageFlags);
    ~CVulkanInternalStagingBuffer();

    const vk::Buffer& GetBuffer() const { return Buffer; }
    const VmaAllocationInfo GetAllocationInfo() const { return AllocationInfo; }
private:
    vk::Buffer Buffer;
    uint64_t Size;
    VmaAllocation Allocation;
    VmaAllocationInfo AllocationInfo;
};

/**
 * A structure holding capabilities, formats and present modes supported by the
 * specified device
 */
struct SVulkanSwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR Capabilities;
	std::vector<vk::SurfaceFormatKHR> Formats;
	std::vector<vk::PresentModeKHR> PresentModes;
};

/**
 * Structure holding queue family indices 
 */
struct SVulkanQueueFamilyIndices
{
	std::optional<uint32_t> Graphics;

	bool IsComplete() const
	{
		return Graphics.has_value();
	}
};

/**
 * Utils functions for flags
 */
namespace VulkanUtil
{
    VmaMemoryUsage BufferUsageFlagsToMemoryUsage(ERSMemoryUsage BufferUsage);
	ZE_FORCEINLINE SVulkanSwapChainSupportDetails QuerySwapChainDetails(const vk::PhysicalDevice& InPhysicalDevice,
        const vk::SurfaceKHR& InSurface);
	SVulkanQueueFamilyIndices GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice);
    ZE_FORCEINLINE vk::Format FormatToVkFormat(const EFormat& InFormat);
    ZE_FORCEINLINE EFormat VkFormatToFormat(const vk::Format& InFormat);
	ZE_FORCEINLINE vk::SampleCountFlagBits SampleCountToVkSampleCount(
		const ESampleCount& InSampleCount);
    ZE_FORCEINLINE vk::ImageAspectFlagBits GetImageAspectFromFormat(const EFormat& InFormat);
    ZE_FORCEINLINE vk::ShaderStageFlagBits ShaderStageToVkShaderStage(const EShaderStage& InShader);
    ZE_FORCEINLINE vk::DescriptorType ShaderParameterTypeToVkDescriptorType(const EShaderParameterType& InType);
	ZE_FORCEINLINE vk::VertexInputRate VertexInputRateToVkVertexInputRate(const EVertexInputRate& InRate);

    /** Rasterizer */
	ZE_FORCEINLINE vk::PolygonMode PolygonModeToVkPolygonMode(EPolygonMode InPolygonMode);
	ZE_FORCEINLINE vk::CullModeFlags CullModeToVkCullMode(ECullMode InCullMode);
	ZE_FORCEINLINE vk::FrontFace FrontFaceToVkFrontFace(EFrontFace InFrontFace);

	vk::CompareOp ComparisonOpToVkCompareOp(const ERSComparisonOp& InOp);

	/** Blend */
	ZE_FORCEINLINE vk::BlendFactor BlendFactorToVkBlendFactor(EBlendFactor InFactor);
	ZE_FORCEINLINE vk::BlendOp BlendOpToVkBlendOp(EBlendOp InOp);
	ZE_FORCEINLINE vk::StencilOp StencilOpToVkStencilOp(EStencilOp InOp);

    /** Render pass related */
    namespace RenderPass
    {
        ZE_FORCEINLINE vk::AttachmentLoadOp AttachmentLoadOpToVkAttachmentLoadOp(
            const ERSRenderPassAttachmentLoadOp& InOp);
		ZE_FORCEINLINE vk::AttachmentStoreOp AttachmentStoreOpToVkAttachmentStoreOp(
			const ERSRenderPassAttachmentStoreOp& InOp);
        ZE_FORCEINLINE vk::ImageLayout AttachmentLayoutToVkImageLayout(
            const ERSRenderPassAttachmentLayout& InLayout);
    }

    /** Sampler */
    ZE_FORCEINLINE vk::Filter FilterToVkFilter(const ERSFilter& InFilter);
    ZE_FORCEINLINE vk::SamplerMipmapMode FilterToVkSamplerMipMapMode(const ERSFilter& InFilter);
    ZE_FORCEINLINE vk::SamplerAddressMode AddressModeToVkSamplerAddressMode(const ERSSamplerAddressMode& InAddressMode);
}

/** Log category */

/** Functions definitions */
namespace VulkanUtil
{
	vk::Format VulkanUtil::FormatToVkFormat(const EFormat& InFormat)
	{
		switch (InFormat)
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
		case EFormat::B8G8R8A8UNorm:
			return vk::Format::eB8G8R8A8Unorm;
		case EFormat::R32G32Sfloat:
			return vk::Format::eR32G32Sfloat;
		case EFormat::R32G32B32Sfloat:
			return vk::Format::eR32G32B32Sfloat;
		case EFormat::R32G32B32A32Sfloat:
			return vk::Format::eR32G32B32A32Sfloat;
		case EFormat::R32G32B32A32Uint:
			return vk::Format::eR32G32B32A32Uint;
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
		default:
			return EFormat::Undefined;
		case vk::Format::eD32Sfloat:
			return EFormat::D32Sfloat;
		case vk::Format::eD32SfloatS8Uint:
			return EFormat::D32SfloatS8Uint;
		case vk::Format::eD24UnormS8Uint:
			return EFormat::D24UnormS8Uint;
		case vk::Format::eR8G8B8A8Unorm:
			return EFormat::R8G8B8A8UNorm;
		case vk::Format::eB8G8R8A8Unorm:
			return EFormat::B8G8R8A8UNorm;
		case vk::Format::eR32G32Sfloat:
			return EFormat::R32G32Sfloat;
		case vk::Format::eR32G32B32Sfloat:
			return EFormat::R32G32B32Sfloat;
		case vk::Format::eR32G32B32A32Sfloat:
			return EFormat::R32G32B32A32Sfloat;
		case vk::Format::eR32G32B32A32Uint:
			return EFormat::R32G32B32A32Uint;
		case vk::Format::eR64Uint:
			return EFormat::R64Uint;
		case vk::Format::eR32Uint:
			return EFormat::R32Uint;
		}
	}

	vk::SampleCountFlagBits VulkanUtil::SampleCountToVkSampleCount(
		const ESampleCount& InSampleCount)
	{
		switch (InSampleCount)
		{
		default:
		case ESampleCount::Sample1:
			return vk::SampleCountFlagBits::e1;
		case ESampleCount::Sample2:
			return vk::SampleCountFlagBits::e2;
		case ESampleCount::Sample4:
			return vk::SampleCountFlagBits::e4;
		case ESampleCount::Sample8:
			return vk::SampleCountFlagBits::e8;
		case ESampleCount::Sample16:
			return vk::SampleCountFlagBits::e16;
		case ESampleCount::Sample32:
			return vk::SampleCountFlagBits::e32;
		case ESampleCount::Sample64:
			return vk::SampleCountFlagBits::e64;
		}
	}

	vk::ShaderStageFlagBits VulkanUtil::ShaderStageToVkShaderStage(const EShaderStage& InShader)
	{
		switch (InShader)
		{
		default:
		case EShaderStage::Vertex:
			return vk::ShaderStageFlagBits::eVertex;
		case EShaderStage::Fragment:
			return vk::ShaderStageFlagBits::eFragment;
		}
	}

	namespace RenderPass
	{

		vk::AttachmentLoadOp VulkanUtil::RenderPass::AttachmentLoadOpToVkAttachmentLoadOp(
			const ERSRenderPassAttachmentLoadOp& InOp)
		{
			switch (InOp)
			{
			default:
			case ERSRenderPassAttachmentLoadOp::DontCare:
				return vk::AttachmentLoadOp::eDontCare;
			case ERSRenderPassAttachmentLoadOp::Clear:
				return vk::AttachmentLoadOp::eClear;
			case ERSRenderPassAttachmentLoadOp::Load:
				return vk::AttachmentLoadOp::eLoad;
			}
		}

		vk::AttachmentStoreOp VulkanUtil::RenderPass::AttachmentStoreOpToVkAttachmentStoreOp(
			const ERSRenderPassAttachmentStoreOp& InOp)
		{
			switch (InOp)
			{
			default:
			case ERSRenderPassAttachmentStoreOp::DontCare:
				return vk::AttachmentStoreOp::eDontCare;
			case ERSRenderPassAttachmentStoreOp::Store:
				return vk::AttachmentStoreOp::eStore;
			}
		}

		vk::ImageLayout VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
			const ERSRenderPassAttachmentLayout& InLayout)
		{
			switch (InLayout)
			{
			default:
			case ERSRenderPassAttachmentLayout::Undefined:
				return vk::ImageLayout::eUndefined;
			case ERSRenderPassAttachmentLayout::ColorAttachment:
				return vk::ImageLayout::eColorAttachmentOptimal;
			case ERSRenderPassAttachmentLayout::ShaderReadOnlyOptimal:
				return vk::ImageLayout::eShaderReadOnlyOptimal;
			case ERSRenderPassAttachmentLayout::DepthStencilAttachment:
				return vk::ImageLayout::eDepthStencilAttachmentOptimal;
			case ERSRenderPassAttachmentLayout::DepthStencilReadOnlyOptimal:
				return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
			case ERSRenderPassAttachmentLayout::Present:
				return vk::ImageLayout::ePresentSrcKHR;
			}
		}

	}
}
