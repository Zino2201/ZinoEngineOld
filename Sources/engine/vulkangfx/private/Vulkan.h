#pragma once

#include "EngineCore.h"

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT ZE_ASSERT
#define VULKAN_HPP_ASSERT_ON_RESULT
#define VMA_ASSERT(expr) ZE_ASSERT(expr)

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#define ZE_ENABLE_VULKAN_DEBUG_FEATURES ZE_DEBUG

const std::vector<const char*> validation_layers =
{
	"VK_LAYER_KHRONOS_validation",
};

#if ZE_ENABLE_VULKAN_DEBUG_FEATURES
constexpr bool enable_validation_layers = true;
#else
constexpr bool enable_validation_layers = false;
#endif

const std::vector<const char*> required_device_extensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};