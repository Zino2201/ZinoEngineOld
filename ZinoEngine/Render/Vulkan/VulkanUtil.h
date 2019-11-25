#pragma once

#include "VulkanCore.h"

/** Vulkan util */
namespace VulkanUtil
{
	SVulkanSwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& InDevice,
		const vk::SurfaceKHR& InSurface);

	SVulkanQueueFamilyIndices GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice,
		const vk::SurfaceKHR& InSurface);

	vk::ShaderStageFlagBits ShaderStageToVkShaderStage(const EShaderStage& InShaderStage);
}