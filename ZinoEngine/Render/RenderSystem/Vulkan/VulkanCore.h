#pragma once

#include "Core/EngineCore.h"
#include "Render/RenderCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

/** Includes */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#endif

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT
#define VMA_ASSERT(expr) must(expr)

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

/** Constants */
const std::vector<const char*> g_VulkanValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool g_VulkanEnableValidationLayers = false;
#else
constexpr bool g_VulkanEnableValidationLayers = true;
#endif

/** Required device extensions */
const std::vector<const char*> g_VulkanRequiredDeviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

constexpr int g_VulkanMaxDescriptorSets = 8; 
constexpr int g_VulkanMaxBindings = 16;
constexpr int g_VulkanNumSetsPerPool = 1;

/** Globals */
extern class CVulkanRenderSystem* g_VulkanRenderSystem;

/** Structures */

/** Queue family indices */
struct SVulkanQueueFamilyIndices
{
	std::optional<uint32_t> Graphics;
	std::optional<uint32_t> Present;

	bool IsComplete() const
	{
		return Graphics.has_value() && Present.has_value();
	}
};

/** Swap chain support details */
struct SVulkanSwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR Capabilities;
	std::vector<vk::SurfaceFormatKHR> Formats;
	std::vector<vk::PresentModeKHR> PresentModes;
};

/**
 * Shader resource binding
 */
struct SVulkanShaderResourceBinding
{
	vk::DescriptorBufferInfo Buffer;
};

/**
 * Shader resource bindings
 */
struct SVulkanShaderResourceBindings
{
	SVulkanShaderResourceBinding Bindings[g_VulkanMaxDescriptorSets][g_VulkanMaxBindings];
};

#include "VulkanUtil.h"