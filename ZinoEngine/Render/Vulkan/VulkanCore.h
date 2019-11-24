#pragma once

#include "Core/EngineCore.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#endif

const std::vector<const char*> g_VulkanValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool g_VulkanEnableValidationLayers = false;
#else
constexpr bool g_VulkanEnableValidationLayers = true;
#endif

#include <vulkan/vulkan.hpp>

/** Required device extensions */
const std::vector<const char*> g_VulkanRequiredDeviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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

extern class CVulkanRenderSystem* g_VulkanRenderSystem;

/** Shader stages */
namespace ShaderStage
{
	enum EStage
	{
		Vertex = 0,
		Fragment = 1,
		NumStages = 2
	};
}

#include "VulkanUtil.h"