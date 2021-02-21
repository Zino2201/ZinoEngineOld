#include "Shader.h"
#include "VulkanBackend.h"
#include "Device.h"
#include "VulkanUtil.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> shaders;
#endif

vk::Result shader_last_result;

std::pair<Result, ResourceHandle> VulkanBackend::shader_create(const ShaderCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<Shader>(ResourceType::Shader,
		*device, in_create_info);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	shaders.insert(handle);
#endif

	return { convert_vk_result(shader_last_result), handle };
}

void VulkanBackend::shader_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Shader>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	shaders.erase(in_handle);
#endif
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
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto shader = shaders.find(in_handle);
	ZE_CHECKF(shader != shaders.end(), "Invalid shader");
#endif

	return get_resource<Shader>(in_handle);
}

}