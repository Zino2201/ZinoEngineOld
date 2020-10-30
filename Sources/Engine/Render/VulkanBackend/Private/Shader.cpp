#include "Shader.h"
#include "VulkanBackend.h"
#include "Device.h"
#include "VulkanUtil.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, Shader> shaders;
vk::Result shader_last_result;

std::pair<Result, ResourceHandle> VulkanBackend::shader_create(const ShaderCreateInfo& in_create_info)
{
	ResourceHandle handle;

	Shader shader(*device, in_create_info);
	if(shader.is_valid())
	{
		handle = create_resource_handle(ResourceType::Shader, 
			static_cast<VkShaderModule>(shader.get_shader()), in_create_info);
		shaders.insert({ handle, std::move(shader)});
	}

	return { convert_vk_result(shader_last_result), handle };
}

void VulkanBackend::shader_destroy(const ResourceHandle& in_handle)
{
	shaders.erase(in_handle);
}

Shader::Shader(Device& in_device, const ShaderCreateInfo& in_create_info) 
	: device(in_device)
{
	auto [result, handle] = device.get_device().createShaderModuleUnique(vk::ShaderModuleCreateInfo(
		vk::ShaderModuleCreateFlags(),
		in_create_info.bytecode.size() * sizeof(uint32_t),
		(in_create_info.bytecode.data())));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create shader: {}",
			vk::to_string(result));

	shader = std::move(handle);
	shader_last_result = result;
}

Shader* Shader::get(const ResourceHandle& in_handle)
{
	auto shader = shaders.find(in_handle);

	if(shader != shaders.end())
		return &shader->second;

	return nullptr;
}

}