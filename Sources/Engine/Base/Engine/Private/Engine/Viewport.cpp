#include "Engine/Viewport.h"
#include "Gfx/Backend.h"

namespace ze
{

Viewport::Viewport(float in_x,
	float in_y,
	float in_width,
	float in_height,
	const gfx::SharedTextureView& in_color_attachment_view) : viewport(in_x,
		in_y, in_width, in_height), color_attachment_view(in_color_attachment_view)
{
	if(!in_color_attachment_view)
	{
		color_attachment = gfx::RenderBackend::get().texture_create(
			gfx::TextureCreateInfo(
				gfx::TextureType::Tex2D,
				gfx::MemoryUsage::GpuOnly,
				gfx::Format::B8G8R8A8Unorm,
				in_width,
				in_height,
				1,
				1,
				1,
				gfx::SampleCountFlagBits::Count1,
				gfx::TextureUsageFlagBits::ColorAttachment | gfx::TextureUsageFlagBits::Sampled |
				gfx::TextureUsageFlagBits::TransferSrc)).second;

		color_attachment_view = gfx::RenderBackend::get().texture_view_create(
			gfx::TextureViewCreateInfo(
				*color_attachment,
				gfx::TextureViewType::Tex2D,
				gfx::Format::B8G8R8A8Unorm,
				gfx::TextureSubresourceRange(
					gfx::TextureAspectFlagBits::Color,
					0,
					1,
					0,
					1)));
	}
}

void Viewport::resize(const float in_width, const float in_height)
{
	viewport.width = in_width;
	viewport.height = in_height;
	
	if(color_attachment)
	{
		color_attachment = gfx::RenderBackend::get().texture_create(
			gfx::TextureCreateInfo(
				gfx::TextureType::Tex2D,
				gfx::MemoryUsage::GpuOnly,
				gfx::Format::B8G8R8A8Unorm,
				in_width,
				in_height,
				1,
				1,
				1,
				gfx::SampleCountFlagBits::Count1,
				gfx::TextureUsageFlagBits::ColorAttachment | gfx::TextureUsageFlagBits::Sampled |
				gfx::TextureUsageFlagBits::TransferSrc)).second;

		color_attachment_view = gfx::RenderBackend::get().texture_view_create(
			gfx::TextureViewCreateInfo(
				*color_attachment,
				gfx::TextureViewType::Tex2D,
				gfx::Format::B8G8R8A8Unorm,
				gfx::TextureSubresourceRange(
					gfx::TextureAspectFlagBits::Color,
					0,
					1,
					0,
					1)));
	}
}

} /* namespace ZE */