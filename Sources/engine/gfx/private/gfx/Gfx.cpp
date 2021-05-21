#include "gfx/Gfx.h"
#include "module/Module.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, gfx)

namespace ze::gfx
{

std::vector<DeviceResourceHandle> static_samplers;

DeviceResourceHandle StaticSamplerBase::register_sampler(DeviceResourceHandle sampler)
{
	return static_samplers.emplace_back(sampler);
}

void StaticSamplerBase::destroy_samplers()
{
	for(const auto& sampler : static_samplers)
		Device::get().destroy_sampler(sampler);
}

}