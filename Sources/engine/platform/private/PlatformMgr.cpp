#include "PlatformMgr.h"
#include "module/Module.h"
#include "zefs/FileStream.h"
#include "zefs/ZEFS.h"
#include "toml++/toml.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, platform);

namespace ze
{

std::vector<std::unique_ptr<PlatformInfo>> platforms;
PlatformInfo* running_platform = nullptr;

PlatformOS get_platform_os_from_name(const std::string& in_name)
{
	if(in_name == "Windows")
		return PlatformOS::Windows;

	if(in_name == "macOS")
		return PlatformOS::MacOS;

	if(in_name == "Linux")
		return PlatformOS::Linux;

	if(in_name == "Android")
		return PlatformOS::Android;

	if(in_name == "iOS")
		return PlatformOS::iOS;
	
	return PlatformOS::Unknown;
}

PlatformEnvironnement get_platform_env_from_name(const std::string& in_name)
{
	if(in_name == "Editor")
		return PlatformEnvironnement::Editor;

	return PlatformEnvironnement::Game;
}
	
void parse_platform_file(const std::filesystem::path& in_file)
{
	filesystem::FileIStream stream(in_file);
	auto root = toml::parse(stream, in_file.stem().string());
	for(const auto& platform : *root["Platform"].as_table())
	{
		auto platform_table = *platform.second.as_table();

		/** Name is required */
		auto name = platform_table["Name"].as_string();
		if(!name)
		{
			logger::error("Invalid platform name for platform {} (file {})",
				platform.first,
				in_file.string());
			continue;
		}
		
		PlatformInfo platform_info(name->get());

		if(auto os = platform_table["OS"].as_string())
			platform_info.os = get_platform_os_from_name(os->get());

		if(auto type = platform_table["Type"].as_string())
			platform_info.env = get_platform_env_from_name(type->get());
		
		register_platform(platform_info);
	}
}
	
void find_platforms()
{
	logger::info("Searching for platforms in Config/Platforms...");

	filesystem::iterate_directories("Config/Platforms", 
		[&](const filesystem::DirectoryEntry& entry)
		{
			/** Treat each file as a platform */
			parse_platform_file("Config/Platforms" / entry.path);
		});

	/** If there is not running platform defined, crash */
	if(!running_platform)
		logger::fatal("Error! Can't find required platform config file for the running platform! Check your Config directory/installation");
}	

void set_running_platform(const std::string& name)
{
	for(const auto& platform : platforms)
	{
		if(platform->name == name)
		{
			running_platform = platform.get();
			logger::info("Running platform | Name: {} | OS: {} | Environnement: {}", 
				platform->name,
				std::to_string(platform->os),
				std::to_string(platform->env));
		}
	}
}
	
void register_platform(const PlatformInfo& in_info)
{
	platforms.emplace_back(std::make_unique<PlatformInfo>(in_info));

	const auto& platform = platforms.back();
	if(platform->os == PlatformInfo::running_os)
	{
#if ZE_WITH_EDITOR
		if(platform->is_editor())
			set_running_platform(platform->name);
#else
		set_current_platform(platform->name);
#endif
	}
	
	logger::info("Registered platform {}", in_info.name);
}

PlatformInfo& get_running_platform()
{
	ZE_ASSERT(running_platform);
	return *running_platform;
}

}
