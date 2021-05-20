#include "Surface.h"
#include <SDL_vulkan.h>
#include "Device.h"
#include "VulkanBackend.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

std::vector<vk::SurfaceKHR> surfaces;

vk::SurfaceKHR get_or_create(void* in_handle)
{
	VkSurfaceKHR surface;

	if (!SDL_Vulkan_CreateSurface(reinterpret_cast<SDL_Window*>(in_handle),
		static_cast<VkInstance>(get_backend().get_instance()),
		&surface))
	{
		ze::logger::error("Failed to create surface: {}", SDL_GetError());
		return vk::SurfaceKHR();
	}

	surfaces.emplace_back(vk::SurfaceKHR(surface));

	return static_cast<vk::SurfaceKHR>(surface);
}

void free_surfaces()
{
	for(const auto& surface : surfaces)
		get_backend().get_instance().destroySurfaceKHR(surface);

	surfaces.clear();
}

}