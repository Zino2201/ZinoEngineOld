#include "Sampler.h"
#include "Device.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, Sampler> samplers;
vk::Result last_sampler_result;

std::pair<Result, ResourceHandle> VulkanBackend::sampler_create(const SamplerCreateInfo& in_create_info)
{
	ResourceHandle handle;

	Sampler sampler(*device, in_create_info);
	if (sampler.is_valid())
	{
		handle = create_resource_handle(ResourceType::Sampler,
			static_cast<VkSampler>(sampler.get_sampler()), in_create_info);
		samplers.insert({ handle, std::move(sampler) });
	}

	return { convert_vk_result(last_sampler_result), handle };
}

void VulkanBackend::sampler_destroy(const ResourceHandle& in_handle)
{
	samplers.erase(in_handle);
}

Sampler::Sampler(Device& in_device, const SamplerCreateInfo& in_create_info)
{
	auto [result, handle] = in_device.get_device().createSamplerUnique(
		vk::SamplerCreateInfo(
			vk::SamplerCreateFlags(),
			convert_filter(in_create_info.mag_filter),
			convert_filter(in_create_info.min_filter),
			convert_mipmapmode(in_create_info.mip_map_mode),
			convert_address_mode(in_create_info.address_mode_u),
			convert_address_mode(in_create_info.address_mode_v),
			convert_address_mode(in_create_info.address_mode_w),
			in_create_info.mip_lod_bias,
			in_create_info.enable_anisotropy,
			in_create_info.max_anisotropy,
			in_create_info.enable_compare_op,
			convert_compare_op(in_create_info.compare_op),
			in_create_info.min_lod,
			in_create_info.max_lod));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create sampler: {}", vk::to_string(result));

	last_sampler_result = result;
	sampler = std::move(handle);
}

Sampler* Sampler::get(const ResourceHandle& in_handle)
{
	auto sampler = samplers.find(in_handle);

	if(sampler != samplers.end())
		return &sampler->second;

	return nullptr;
}

}