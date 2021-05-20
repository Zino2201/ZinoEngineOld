#pragma once

#include "Vulkan.h"
#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class Shader
{
public:
	Shader(Device& in_device, const ShaderCreateInfo& in_create_info);

	static Shader* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!shader ;}
	ZE_FORCEINLINE vk::ShaderModule& get_shader() { return *shader; }
private:
	Device& device;
	vk::UniqueShaderModule shader;
};

}