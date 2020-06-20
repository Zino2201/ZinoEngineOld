#pragma once

#include "EngineCore.h"
#include <optional>
#include "Render/RenderSystem/RenderSystemResources.h"

/** OOF */
using namespace ZE;
using namespace ZE::Math;

VULKANRENDERSYSTEM_API extern class CVulkanRenderSystem* GVulkanRenderSystem;
VULKANRENDERSYSTEM_API extern class CVulkanRenderSystemContext* GRenderSystemContext;

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT
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
constexpr bool GVulkanEnableValidationLayers = false; // false
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
	CVulkanDeviceResource(CVulkanDevice* InDevice) : Device(InDevice) {}
    virtual ~CVulkanDeviceResource() = default;
protected:
    CVulkanDevice* Device;
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
    CVulkanInternalStagingBuffer(CVulkanDevice* InDevice,
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
	SVulkanSwapChainSupportDetails QuerySwapChainDetails(const vk::PhysicalDevice& InPhysicalDevice,
        const vk::SurfaceKHR& InSurface);
	SVulkanQueueFamilyIndices GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice);
    vk::Format FormatToVkFormat(const EFormat& InFormat);
    EFormat VkFormatToFormat(const vk::Format& InFormat);
	vk::SampleCountFlagBits SampleCountToVkSampleCount(
		const ESampleCount& InSampleCount);
    vk::ImageAspectFlagBits GetImageAspectFromFormat(const EFormat& InFormat);
    vk::ShaderStageFlagBits ShaderStageToVkShaderStage(const EShaderStage& InShader);
    vk::DescriptorType ShaderParameterTypeToVkDescriptorType(const EShaderParameterType& InType);
	vk::VertexInputRate VertexInputRateToVkVertexInputRate(const EVertexInputRate& InRate);

    /** Rasterizer */
	vk::PolygonMode PolygonModeToVkPolygonMode(EPolygonMode InPolygonMode);
	vk::CullModeFlags CullModeToVkCullMode(ECullMode InCullMode);
	vk::FrontFace FrontFaceToVkFrontFace(EFrontFace InFrontFace);

	vk::CompareOp ComparisonOpToVkCompareOp(const ERSComparisonOp& InOp);

	/** Blend */
	vk::BlendFactor BlendFactorToVkBlendFactor(EBlendFactor InFactor);
	vk::BlendOp BlendOpToVkBlendOp(EBlendOp InOp);
	vk::StencilOp StencilOpToVkStencilOp(EStencilOp InOp);

    /** Render pass related */
    namespace RenderPass
    {
        vk::AttachmentLoadOp AttachmentLoadOpToVkAttachmentLoadOp(
            const ERSRenderPassAttachmentLoadOp& InOp);
		vk::AttachmentStoreOp AttachmentStoreOpToVkAttachmentStoreOp(
			const ERSRenderPassAttachmentStoreOp& InOp);
        vk::ImageLayout AttachmentLayoutToVkImageLayout(
            const ERSRenderPassAttachmentLayout& InLayout);
    }

    /** Sampler */
    vk::Filter FilterToVkFilter(const ERSFilter& InFilter);
    vk::SamplerMipmapMode FilterToVkSamplerMipMapMode(const ERSFilter& InFilter);
    vk::SamplerAddressMode AddressModeToVkSamplerAddressMode(const ERSSamplerAddressMode& InAddressMode);
}

/** Log category */
DECLARE_LOG_CATEGORY(VulkanRS);
