#include "Surface.h"
#include <SDL2/SDL_vulkan.h>
#include "Device.h"
#include "VulkanBackend.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<void*, vk::UniqueSurfaceKHR> surfaces;

vk::SurfaceKHR get_or_create(void* in_handle)
{
	auto it = surfaces.find(in_handle);
	if(it != surfaces.end())
		return *it->second;

	VkSurfaceKHR surface;

	if (!SDL_Vulkan_CreateSurface(reinterpret_cast<SDL_Window*>(in_handle),
		static_cast<VkInstance>(get_backend().get_instance()),
		&surface))
	{
		ze::logger::error("Failed to create surface: {}", SDL_GetError());
		return vk::SurfaceKHR();
	}

	surfaces.insert({ in_handle, vk::UniqueSurfaceKHR(surface, vk::ObjectDestroy<vk::Instance,
		VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(get_backend().get_instance(), nullptr, vk::DispatchLoaderStatic())) });

	return static_cast<vk::SurfaceKHR>(surface);
}

void free_surfaces()
{
	surfaces.clear();
}

}