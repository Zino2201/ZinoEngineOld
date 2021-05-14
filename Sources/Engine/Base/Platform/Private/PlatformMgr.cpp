#include "PlatformMgr.h"
#include "PlatformMgr.h"
#include "Module/Module.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, Platform);

namespace ze
{

std::vector<std::unique_ptr<PlatformInfo>> platforms;
PlatformInfo* current_platform = nullptr;

void register_platform(const PlatformInfo& in_info)
{
	platforms.emplace_back(std::make_unique<PlatformInfo>(in_info));

	logger::info("Registered platform {}", in_info.name);
}

void set_current_platform(const std::string& name)
{
	for(const auto& platform : platforms)
	{
		if(platform->name == name)
		{
			current_platform = platform.get();
			logger::info("Current platform | Name: {} | OS: {} | Environnement: {}", 
				platform->name,
				std::to_string(platform->os),
				std::to_string(platform->env));
		}
	}
}

PlatformInfo& get_current_platform()
{
	ZE_CHECK(current_platform);
	return *current_platform;
}

}
