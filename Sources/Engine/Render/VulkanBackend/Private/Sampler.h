#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class Sampler
{
public:
	Sampler(Device& in_device, const SamplerCreateInfo& in_create_info);
	
	static Sampler* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE vk::Sampler& get_sampler() { return *sampler; }
	ZE_FORCEINLINE bool is_valid() const { return !!sampler ;}
private:
	vk::UniqueSampler sampler;
};

}