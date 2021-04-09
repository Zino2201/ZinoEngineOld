#include "Sampler.h"
#include "Device.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> samplers;
#endif

vk::Result last_sampler_result;

std::pair<Result, ResourceHandle> VulkanBackend::sampler_create(const SamplerCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<Sampler>(*device, in_create_info);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	samplers.insert(handle);
#endif

	return { convert_vk_result(last_sampler_result), handle };
}

void VulkanBackend::sampler_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Sampler>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	samplers.erase(in_handle);
#endif
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
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto sampler = samplers.find(in_handle);
	ZE_CHECKF(sampler != samplers.end(), "Invalid sampler");
#endif

	return get_resource<Sampler>(in_handle);;
}

}