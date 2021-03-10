#pragma once

#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

VULKANBACKEND_API OwnerPtr<Backend> create_vulkan_backend();

}