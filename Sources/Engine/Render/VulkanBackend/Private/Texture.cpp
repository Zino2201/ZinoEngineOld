#include "Texture.h"
#include "Device.h"
#include <robin_hood.h>
#include "VulkanUtil.h"
#include "VulkanBackend.h"

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, Texture> textures;
robin_hood::unordered_map<ResourceHandle, TextureView> texture_views;
vk::Result last_result;

std::pair<Result, ResourceHandle> VulkanBackend::texture_create(const TextureCreateInfo& in_create_info)
{
	ResourceHandle handle;

	Texture texture(*device, in_create_info);
	if(texture.is_valid())
	{
		handle = create_resource_handle(ResourceType::Texture, 
			static_cast<VkImage>(texture.get_image()), in_create_info);
		textures.insert({ handle, std::move(texture)});
	}

	return { convert_vk_result(last_result), handle };
}

std::pair<Result, ResourceHandle> VulkanBackend::texture_create(const vk::Image& in_image,
	const TextureCreateInfo& in_create_info)
{
	ResourceHandle handle;

	Texture texture(*device, in_image, in_create_info);
	if(texture.is_valid())
	{
		handle = create_resource_handle(ResourceType::Texture, 
			static_cast<VkImage>(texture.get_image()), in_create_info);
		textures.insert({ handle, std::move(texture)});
	}

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
	textures.erase(in_handle);
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
	auto tex = textures.find(in_handle);

	if(tex != textures.end())
		return &tex->second;
	
	return nullptr;
}

ResourceHandle VulkanBackend::texture_view_create(const TextureViewCreateInfo& in_create_info)
{
	ResourceHandle handle;

	TextureView view(*device, in_create_info);
	if(view.is_valid())
	{
		handle = create_resource_handle(ResourceType::TextureView,
			static_cast<VkImageView>(view.get_image_view()), in_create_info);
		texture_views.insert({ handle, std::move(view) });
	}

	return handle;
}

ResourceHandle VulkanBackend::texture_view_create(const vk::Image& in_image,
	const TextureViewCreateInfo& in_create_info)
{
	ResourceHandle handle;

	TextureView view(*device, in_image, in_create_info);
	if(view.is_valid())
	{
		handle = create_resource_handle(ResourceType::TextureView,
			static_cast<VkImageView>(view.get_image_view()), in_create_info);
		texture_views.insert({ handle, std::move(view) });
	}

	return handle;
}

void VulkanBackend::texture_view_destroy(const ResourceHandle& in_handle)
{
	texture_views.erase(in_handle);
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
	auto tex = texture_views.find(in_handle);

	if(tex != texture_views.end())
		return &tex->second;
	
	return nullptr;
}

}