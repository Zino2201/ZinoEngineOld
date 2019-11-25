#include "VulkanDevice.h"
#include "Render/Window.h"
#include "Core/Engine.h"
#include "VulkanRenderSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "VulkanQueue.h"

CVulkanDevice::CVulkanDevice(const vk::PhysicalDevice& InPhysDevice)
	: PhysicalDevice(InPhysDevice)
{
	CWindow* Window = CEngine::Get().GetWindow();

	QueueFamilyIndices = VulkanUtil::GetQueueFamilyIndices(PhysicalDevice,
		g_VulkanRenderSystem->GetSurface());

	/** Create logical device */
	{
		/** Queue create infos */

		std::set<uint32_t> UniqueQueueFamilies =
		{
			QueueFamilyIndices.Graphics.value(),
			QueueFamilyIndices.Present.value()
		};

		std::vector<vk::DeviceQueueCreateInfo> QueueCreateInfos;

		float QueuePriority = 1.f;
		for (uint32_t QueueFamily : UniqueQueueFamilies)
		{
			QueueCreateInfos.emplace_back(
				vk::DeviceQueueCreateFlags(),
				QueueFamily,
				1,
				&QueuePriority);
		}

		/** Create device */
		vk::DeviceCreateInfo CreateInfos(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>(QueueCreateInfos.size()),
			QueueCreateInfos.data(),
			g_VulkanEnableValidationLayers ? static_cast<uint32_t>(g_VulkanValidationLayers.size()) : 0,
			g_VulkanEnableValidationLayers ? g_VulkanValidationLayers.data() : 0,
			static_cast<uint32_t>(g_VulkanRequiredDeviceExtensions.size()),
			g_VulkanRequiredDeviceExtensions.data());

		Device = PhysicalDevice.createDeviceUnique(CreateInfos);
		if (!Device)
			LOG(ELogSeverity::Fatal, "Failed to create logical device")

		/** Get queues */
		GraphicsQueue = std::make_unique<CVulkanQueue>(this, QueueFamilyIndices.Graphics.value());
		PresentQueue = std::make_unique<CVulkanQueue>(this, QueueFamilyIndices.Present.value());
	}
}

CVulkanDevice::~CVulkanDevice() 
{
	
}