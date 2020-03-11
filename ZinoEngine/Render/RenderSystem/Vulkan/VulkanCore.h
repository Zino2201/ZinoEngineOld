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

#ifndef VULKAN_ENABLE_VALIDATION_LAYERS
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

// Global resources
#define GLOBAL_SET 0

// Material resources
#define MATERIAL_SET 1

// Instance resources
#define INSTANCE_SET 2

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

/** Stats */
DECLARE_STAT_GROUP(VulkanRendering)

#include "VulkanUtil.h"