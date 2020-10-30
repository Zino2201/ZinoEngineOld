#pragma once

#include "Vulkan.h"

namespace ze::gfx::vulkan
{
	
vk::SurfaceKHR get_or_create(void* in_handle);
void free_surfaces();

}