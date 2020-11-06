#define VMA_IMPLEMENTATION
#include "VulkanBackend.h"
#include "Gfx/Vulkan/Backend.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "Module/Module.h"
#include "Device.h"
#include <set>	
#include "Buffer.h"
#include "Surface.h"
#include "RenderPass.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, VulkanBackend);

namespace ze::gfx::vulkan
{

VulkanBackend* backend = nullptr;

OwnerPtr<RenderBackend> create_vulkan_backend()
{
	return new VulkanBackend;
}

/**
 * Validation layers callback
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT InSeverity,
	VkDebugUtilsMessageTypeFlagsEXT InMessageType,
	const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
	void* InUserData)
{
	switch (InSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		ze::logger::info(InCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		ze::logger::verbose(InCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		ze::logger::warn(InCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		ze::logger::fatal(InCallbackData->pMessage);
		ZE_DEBUGBREAK();
		return VK_TRUE;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		break;
	}

	return VK_FALSE;
}

vk::Result CreateDebugUtilsMessengerEXT(
	vk::Instance instance,
	const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const vk::AllocationCallbacks* pAllocator,
	vk::DebugUtilsMessengerEXT* pDebugMessenger)
{

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return static_cast<vk::Result>(func(
			static_cast<VkInstance>(instance),
			reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(pCreateInfo),
			reinterpret_cast<const VkAllocationCallbacks*>(pAllocator),
			reinterpret_cast<VkDebugUtilsMessengerEXT*>(pDebugMessenger)));
	}
	else
	{
		return vk::Result::eErrorExtensionNotPresent;
	}
}

void DestroyDebugUtilsMessengerEXT(
	vk::Instance instance,
	vk::DebugUtilsMessengerEXT  debugMessenger,
	const vk::AllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(
			static_cast<VkInstance>(instance),
			static_cast<VkDebugUtilsMessengerEXT>(debugMessenger),
			reinterpret_cast<const VkAllocationCallbacks*>(pAllocator));
	}
}

std::vector<const char*> get_required_extensions()
{
	SDL_Window* dummy = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);
	if (!dummy)
	{
		ze::logger::error("Failed to get required extensions: {}", SDL_GetError());
		return {};
	}

	uint32_t extension_count = 0;
	SDL_Vulkan_GetInstanceExtensions(dummy, &extension_count, nullptr);
	std::vector<const char*> extension_names(extension_count);
	SDL_Vulkan_GetInstanceExtensions(dummy, &extension_count, extension_names.data());

	if (enable_validation_layers)
		extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	SDL_DestroyWindow(dummy);

	return extension_names;
}

VulkanBackend::~VulkanBackend()
{
	destroy_framebuffers();
	free_surfaces();

	DestroyDebugUtilsMessengerEXT(
		*instance,
		messenger, nullptr);
}

std::pair<bool, std::string> VulkanBackend::initialize()
{
	backend = this;

	{
		vk::ApplicationInfo app_infos(
			nullptr,
			0, 
			nullptr, 
			0, 
			VK_API_VERSION_1_1);

		if(enable_validation_layers)
			ze::logger::warn("Vulkan validation layers are enabled, performances will be degraded !");

		/**
		 * Get the list of required extensions and check if the host supports them
		 */
		std::vector<const char*> required_extensions = get_required_extensions();
		std::vector<vk::ExtensionProperties> supported_extensions =
				vk::enumerateInstanceExtensionProperties().value;

		ze::logger::verbose("--- Vulkan Extensions ---");
		int found_extension_count = 0;
		for (const char* required_extension : required_extensions)
		{
			bool found = false;

			for (const vk::ExtensionProperties& extension : supported_extensions)
			{
				if (found_extension_count == 0)
					ze::logger::verbose("- {}", extension.extensionName);

				if (strcmp(extension.extensionName, required_extension) == 0)
				{
					found = true;
					found_extension_count++;
					break;
				}
			}

			if (!found)
				break;
		}

		if (found_extension_count < required_extensions.size())
		{
			ze::logger::error("This system doesn't support required Vulkan extensions ({} found, {} required)",
				found_extension_count,
				required_extensions.size());
			return { false, "Host system doesn't support required Vulkan extensions" };
		}
		else
		{
			ze::logger::info("Found all required Vulkan extensions!");
		}

		/** Create the actual instance */
		vk::InstanceCreateInfo create_infos(
				vk::InstanceCreateFlags(),
				&app_infos,
				enable_validation_layers ? static_cast<uint32_t>(validation_layers.size()) : 0,
				enable_validation_layers ? validation_layers.data() : 0,
				static_cast<uint32_t>(required_extensions.size()),
				required_extensions.data());

		auto [result, instance_handle] = vk::createInstanceUnique(create_infos);
		if (result != vk::Result::eSuccess)
		{
			ze::logger::error("Failed to create Vulkan instance: {}",
				vk::to_string(result).c_str());
			return { false, "Failed to create Vulkan instance" };
		}

		instance = std::move(instance_handle);
	}

	/** Create debug callback */
	if (enable_validation_layers)
	{
		vk::DebugUtilsMessengerCreateInfoEXT create_infos(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
			VkDebugCallback);

		vk::Result result = CreateDebugUtilsMessengerEXT(*instance,
			&create_infos, nullptr, &messenger);
		if(result != vk::Result::eSuccess)
			ze::logger::error("Failed to create debug utils messenger");
	}

	/** Select a physical device and create a logical device */ 
	{
		std::vector<vk::PhysicalDevice> physical_devices =
			instance->enumeratePhysicalDevices().value;

		vk::PhysicalDevice phys_device_to_use;

		for (const vk::PhysicalDevice& phys_device : physical_devices)
		{
			if (is_phys_device_usable(phys_device))
			{
				phys_device_to_use = phys_device;
				break;
			}
		}

		if (!phys_device_to_use)
			return { false, "No Vulkan compatible GPU found" };

		device = std::make_unique<Device>(phys_device_to_use);
		if(!device->is_valid())
		{
			return { false, "Failed to create device" };
		}

		device->create_queues();
	}

	return { true, "" };
}

void VulkanBackend::device_wait_idle()
{
	device->get_device().waitIdle();
}

void VulkanBackend::new_frame()
{
	update_framebuffers();
}

bool VulkanBackend::is_phys_device_usable(const vk::PhysicalDevice& in_device) const
{
	/** Check required extensions */
	std::vector<vk::ExtensionProperties> available_extensions =
		in_device.enumerateDeviceExtensionProperties().value;

	std::set<std::string> required_extensions(required_device_extensions.begin(),
		required_device_extensions.end());

	for (const vk::ExtensionProperties& extension : available_extensions)
		required_extensions.erase(extension.extensionName);

	return required_extensions.empty();
}

VulkanBackend& get_backend()
{
	return *backend;
}

}