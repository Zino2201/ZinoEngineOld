#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystemContext.h"
#include "Module/Module.h"
#include "Render/VulkanRenderSystem/VulkanCore.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanPipelineLayout.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <set>
#include <assert.h>

DEFINE_MODULE(CDefaultModule, VulkanRenderSystem)

extern class CVulkanRenderSystem* GVulkanRenderSystem = nullptr;
extern class CVulkanRenderSystemContext* GRenderSystemContext = nullptr;

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
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		LOG(ELogSeverity::Debug, VulkanRS, InCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LOG(ELogSeverity::Warn, VulkanRS, InCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LOG(ELogSeverity::Fatal, VulkanRS, InCallbackData->pMessage);
		__debugbreak();
		return VK_TRUE;
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

void CVulkanRenderSystem::Initialize()
{
	GRenderSystem = this;
	GVulkanRenderSystem = this;

	/**
	 * Initialize vulkan
	 */
	{
		vk::ApplicationInfo ApplicationInfos(nullptr,
			0, nullptr, 0, VK_API_VERSION_1_1);

		LOG(ELogSeverity::Info, VulkanRS, "Validation layers: %s",
			GVulkanEnableValidationLayers ? "Yes": "No");

		if constexpr(GVulkanEnableValidationLayers)
		{
			LOG(ELogSeverity::Warn, VulkanRS, "Validation layers are enabled! Except bad performances !");
		}

		/** Get required extensions */
		std::vector<const char*> RequiredExtensions = GetRequiredExtensions();

		/** Get supported extensions */
		std::vector<vk::ExtensionProperties> SupportedExtensions =
			vk::enumerateInstanceExtensionProperties().value;

		/** Check if required extensions are supported */
		LOG(ELogSeverity::Debug, VulkanRS, "--- Extensions ---");
		int FoundExtensionCount = 0;
		for (const char* RequiredExtension : RequiredExtensions)
		{
			bool Found = false;

			for (const vk::ExtensionProperties& Extension : SupportedExtensions)
			{
				if (FoundExtensionCount == 0)
					LOG(ELogSeverity::Debug, VulkanRS,
						"- %s", Extension.extensionName);

				if (strcmp(Extension.extensionName, RequiredExtension) == 0)
				{
					Found = true;
					FoundExtensionCount++;
					break;
				}
			}

			if (!Found)
				break;
		}

		if (FoundExtensionCount < RequiredExtensions.size())
		{
			LOG(ELogSeverity::Fatal, VulkanRS,
				"This system doesn't support required extensions (%d found, %d required)",
				FoundExtensionCount,
				RequiredExtensions.size());
		}
		else
		{
			LOG(ELogSeverity::Info, VulkanRS,
				"Found all required extensions!");
		}

		vk::InstanceCreateInfo CreateInfos(
			vk::InstanceCreateFlags(),
			&ApplicationInfos,
			GVulkanEnableValidationLayers ? static_cast<uint32_t>(GVulkanValidationLayers.size()) : 0,
			GVulkanEnableValidationLayers ? GVulkanValidationLayers.data() : 0,
			static_cast<uint32_t>(RequiredExtensions.size()),
			RequiredExtensions.data());
		auto [Result, InstanceHandle] = vk::createInstance(CreateInfos);
		if (Result != vk::Result::eSuccess)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create Vulkan instance: %s",
				vk::to_string(Result).c_str());
		Instance.reset(InstanceHandle);
	}

	/** Create debug callback */
	if (GVulkanEnableValidationLayers)
	{
		vk::DebugUtilsMessengerCreateInfoEXT CreateInfos(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
			VkDebugCallback);

		CreateDebugUtilsMessengerEXT(*Instance,
			&CreateInfos, nullptr, &Callback);
	}

	/** Select a physical device and create a logical device */
	{
		std::vector<vk::PhysicalDevice> PhysicalDevices =
			Instance->enumeratePhysicalDevices().value;
		vk::PhysicalDevice PhysicalDevice;

		for (const vk::PhysicalDevice& PhysDevice : PhysicalDevices)
		{
			if (IsDeviceUseable(PhysDevice))
			{
				PhysicalDevice = PhysDevice;
				break;
			}
		}

		if (!PhysicalDevice)
			LOG(ELogSeverity::Fatal, VulkanRS, 
				"Failed to find a Vulkan compatible GPU.");

		Device = std::make_unique<CVulkanDevice>(PhysicalDevice);
		GRenderSystemContext = Device->GetContext();
	}
}

void CVulkanRenderSystem::Destroy()
{
	GRenderSystem = nullptr;

	if (GVulkanEnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(*Instance, Callback, nullptr);
}

void CVulkanRenderSystem::NewFrame()
{
	for(auto& [Unused, Layout] : Device->GetPipelineLayoutMgr()->GetLayoutMap())
	{
		Layout->GetSetManager().NewFrame();
	}
}

void CVulkanRenderSystem::WaitGPU()
{
	Device->WaitIdle();
}

std::vector<const char*> CVulkanRenderSystem::GetRequiredExtensions() const
{
	SDL_Window* DummyWindow = SDL_CreateWindow("", 0, 0, 2, 2, SDL_WINDOW_VULKAN);

	uint32_t ExtensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(DummyWindow, &ExtensionCount, nullptr);
	std::vector<const char*> ExtensionNames(ExtensionCount);
	SDL_Vulkan_GetInstanceExtensions(DummyWindow, &ExtensionCount, ExtensionNames.data());

	if (GVulkanEnableValidationLayers)
		ExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	SDL_DestroyWindow(DummyWindow);

	return ExtensionNames;
}

bool CVulkanRenderSystem::IsDeviceUseable(const vk::PhysicalDevice& InDevice) const
{
	/** Check required extensions */
	std::vector<vk::ExtensionProperties> AvailableExtensions =
		InDevice.enumerateDeviceExtensionProperties().value;

	std::set<std::string> RequiredExtensions(GVulkanRequiredDeviceExtensions.begin(),
		GVulkanRequiredDeviceExtensions.end());

	for (const vk::ExtensionProperties& Extension : AvailableExtensions)
		RequiredExtensions.erase(Extension.extensionName);

	return RequiredExtensions.empty();
}

SVulkanQueueFamilyIndices VulkanUtil::GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice)
{
	SVulkanQueueFamilyIndices Indices;

	std::vector<vk::QueueFamilyProperties> QueueFamilies = InDevice.getQueueFamilyProperties();

	/** Present queue will be created later */
	int i = 0;
	for (const vk::QueueFamilyProperties& QueueFamily : QueueFamilies)
	{
		if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			Indices.Graphics = i;
		}

		if (Indices.IsComplete())
			break;

		i++;
	}

	return Indices;
}

#include "Render/VulkanRenderSystem/VulkanRenderSystemInterface.h"

IRenderSystem* CreateVulkanRenderSystem()
{
	return new CVulkanRenderSystem;
}

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
	switch(InSampleCount)
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