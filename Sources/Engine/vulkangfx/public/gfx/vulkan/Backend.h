#pragma once

#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

OwnerPtr<Backend> create_vulkan_backend();

}