#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class Texture
{
public:
	Texture(Device& in_device, const TextureCreateInfo& in_create_infos);
	Texture(Device& in_device, const vk::Image& in_image,
		const TextureCreateInfo& in_create_infos);
	~Texture();

	Texture(Texture&& other) :
		device(other.device),
		own_image(std::exchange(other.own_image, false)),
		image(std::exchange(other.image, vk::Image())),
		allocation(std::exchange(other.allocation, VmaAllocation{})) {}

	static Texture* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!image; }
	ZE_FORCEINLINE vk::Image& get_image() { return image; }
private:
	Device& device;
	bool own_image;
	vk::Image image;
	VmaAllocation allocation;
};

class TextureView
{
public:
	TextureView(Device& in_device, const TextureViewCreateInfo& in_create_info);
	TextureView(Device& in_device, const vk::Image& in_image,
		const TextureViewCreateInfo& in_create_info);

	static TextureView* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!image_view; }
	ZE_FORCEINLINE vk::ImageView& get_image_view() { return *image_view; }
private:
	vk::UniqueImageView image_view;
};

}