#pragma once

#include "Gfx/Resource.h"
#include "ImGui/ImGui.h"

namespace ze::ui::imgui
{

IMGUI_API bool initialize(const gfx::ResourceHandle& in_cmd_list, const gfx::ResourceHandle& in_renderpass);
IMGUI_API void destroy();

/**
 * Update ImGui data
 */
IMGUI_API void update();

/**
 * Draw ImGui data
 */
IMGUI_API void draw(const gfx::ResourceHandle& in_cmd_list);

}