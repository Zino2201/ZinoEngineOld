#include "Texture.h"
#include "Device.h"
#include <robin_hood.h>
#include "VulkanUtil.h"
#include "VulkanBackend.h"

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> textures;
robin_hood::unordered_set<ResourceHandle> texture_views;
#endif
vk::Result last_result;

std::pair<Result, ResourceHandle> VulkanBackend::texture_create(const TextureCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<Texture>(*device, in_create_info);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	textures.insert(handle);
#endif

	return { convert_vk_result(last_result), handle };
}

std::pair<Result, ResourceHandle> VulkanBackend::texture_create(const vk::Image& in_image,
	const TextureCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<Texture>(*device, in_image, in_create_info);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	textures.insert(handle);
#endif

	return { convert_vk_result(last_result), handle };
}

Texture::Texture(Device& in_device, const TextureCreateInfo& in_create_infos) :
	device(in_device), own_image(true)
{
	vk::ImageCreateInfo create_info(
		vk::ImageCreateFlags(),
		convert_texture_type(in_create_infos.type),
		convert_format(in_create_infos.format),
		vk::Extent3D(in_create_infos.width,
			in_create_infos.height, in_create_infos.depth),
		in_create_infos.mip_levels,
		in_create_infos.array_layers,
		convert_sample_count_bit(in_create_infos.sample_count),
		vk::ImageTiling::eOptimal,
		convert_texture_usage_flags(in_create_infos.usage_flags),
		vk::SharingMode::eExclusive,
		{},
		convert_texture_layout(in_create_infos.initial_layout));

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = convert_memory_usage(in_create_infos.mem_usage);

	vk::Result result = static_cast<vk::Result>(vmaCreateImage(
		device.get_allocator(),
		reinterpret_cast<VkImageCreateInfo*>(&create_info),
		&alloc_info,
		reinterpret_cast<VkImage*>(&image),
		&allocation,
		nullptr));
	if(result != vk::Result::eSuccess)
	{
		ze::logger::error("Failed to create texture: {}", vk::to_string(result));
	}

	last_result = result;
}

void VulkanBackend::texture_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Texture>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	textures.erase(in_handle);
#endif
}

Texture::Texture(Device& in_device, const vk::Image& in_image,
	const TextureCreateInfo& in_create_infos) :
	device(in_device), own_image(false)
{
	image = in_image;
}

Texture::~Texture()
{
	if(own_image)
	{
		vmaDestroyImage(device.get_allocator(),
			image,
			allocation);
	}
}

Texture* Texture::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto tex = textures.find(in_handle);
	ZE_CHECKF(tex != textures.end(), "Invalid texture");
#endif

	return get_resource<Texture>(in_handle);
}

std::pair<Result, ResourceHandle> VulkanBackend::texture_view_create(const TextureViewCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<TextureView>(*device, in_create_info);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	texture_views.insert(handle);
#endif

	return { convert_vk_result(last_result), handle };
}

std::pair<Result, ResourceHandle> VulkanBackend::texture_view_create(const vk::Image& in_image,
	const TextureViewCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<TextureView>(*device, in_image, in_create_info);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	texture_views.insert(handle);
#endif

	return { convert_vk_result(last_result), handle };
}

void VulkanBackend::texture_view_destroy(const ResourceHandle& in_handle)
{
	delete_resource<TextureView>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	texture_views.erase(in_handle);
#endif
}

TextureView::TextureView(Device& in_device, const TextureViewCreateInfo& in_create_info)
{
	Texture* tex = Texture::get(in_create_info.texture);
	if(!tex)
		return;

	auto [result, handle] = in_device.get_device().createImageViewUnique(
		vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			tex->get_image(),
			convert_texture_view_type(in_create_info.type),
			convert_format(in_create_info.format),
			vk::ComponentMapping(),
			convert_texture_subresource_range(in_create_info.subresource_range)));
	last_result = result;
	if(!handle)
	{
		ze::logger::error("Failed to create texture view: {}", vk::to_string(result));
	}

	image_view = std::move(handle);
}

TextureView::TextureView(Device& in_device, const vk::Image& in_image,
	const TextureViewCreateInfo& in_create_info)
{
	auto [result, handle] = in_device.get_device().createImageViewUnique(
		vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			in_image,
			convert_texture_view_type(in_create_info.type),
			convert_format(in_create_info.format),
			vk::ComponentMapping(),
			convert_texture_subresource_range(in_create_info.subresource_range)));
	if(!handle)
	{
		ze::logger::error("Failed to create texture view: {}", vk::to_string(result));
	}

	image_view = std::move(handle);
}

TextureView* TextureView::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto tex = texture_views.find(in_handle);
	ZE_CHECKF(tex != texture_views.end(), "Invalid texture view");
#endif

	return get_resource<TextureView>(in_handle);
}

}