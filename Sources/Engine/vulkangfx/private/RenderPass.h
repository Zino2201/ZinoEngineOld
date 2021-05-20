#pragma once

#include "Vulkan.h"
#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class RenderPass
{
public:
	RenderPass(Device& in_device, 
		const RenderPassCreateInfo& in_create_info);

	static RenderPass* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!render_pass; }
	ZE_FORCEINLINE vk::RenderPass& get_render_pass() { return *render_pass; }
	ZE_FORCEINLINE const RenderPassCreateInfo& get_create_info() { return create_info; }
private:
	Device& device;
	vk::UniqueRenderPass render_pass;
	RenderPassCreateInfo create_info;
};

void destroy_framebuffers();
vk::Framebuffer get_or_create_framebuffer(Device& in_device, const Framebuffer& in_framebuffer,
	const vk::RenderPass& in_render_pass);
void update_framebuffers();

}