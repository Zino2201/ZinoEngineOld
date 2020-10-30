#include "Engine/Viewport.h"
#include "Gfx/Backend.h"

namespace ze
{

Viewport::Viewport(void* in_parent_window,
	uint32_t in_width,
	uint32_t in_height,
	gfx::ResourceHandle in_render_target) : parent_window(in_parent_window),
	width(in_width), height(in_height), render_target(in_render_target)
{
	ZE_CHECKF(render_target.type == gfx::ResourceType::TextureView, "Viewport requires an render target");
}

void Viewport::begin()
{
	
}

void Viewport::end()
{
}

} /* namespace ZE */