#pragma once

#include "EngineCore.h"

namespace ze
{

/** Operating System running the platform */
enum class PlatformOS
{
	Unknown,
	Windows,
	MacOS,
	Linux,
	Android,
	iOS,
};

/** Environnement the platform runs on */
enum class PlatformEnvironnement
{
	Editor,
	Game
};

struct PlatformInfo
{
#if ZE_PLATFORM(WINDOWS)
	static constexpr PlatformOS running_os = PlatformOS::Windows;
#elif ZE_PLATFORM(OSX)
	static constexpr PlatformOS running_os = PlatformOS::MacOS;
#elif ZE_PLATFORM(LINUX)
	static constexpr PlatformOS running_os = PlatformOS::Linux;
#elif ZE_PLATFORM(ANDROID)
	static constexpr PlatformOS running_os = PlatformOS::Android;
#else
	static constexpr PlatformOS running_os = PlatformOS::Unknown;
#endif
	
	/** Base informations */
	std::string name;
	PlatformOS os;
	PlatformEnvironnement env;
	bool is_little_endian;

	PlatformInfo(const std::string& in_name, 
		const PlatformOS in_os = PlatformOS::Unknown,
		const PlatformEnvironnement in_env = PlatformEnvironnement::Game,
		const bool in_is_little_endian = (std::endian::native == std::endian::little))
		: name(in_name), os(in_os), env(in_env), is_little_endian(in_is_little_endian) {}
	
	bool is_editor() const { return env == PlatformEnvironnement::Editor; }
};

}

namespace std
{

inline std::string to_string(const ze::PlatformOS& os)
{
	switch (os)
	{
	case ze::PlatformOS::Unknown:
		return "Unknown";
	case ze::PlatformOS::Windows:
		return "Windows";
	case ze::PlatformOS::MacOS:
		return "macOS";
	case ze::PlatformOS::Linux:
		return "Linux";
	case ze::PlatformOS::Android:
		return "Android";
	case ze::PlatformOS::iOS:
		return "iOS";
	}
}

inline std::string to_string(const ze::PlatformEnvironnement& env)
{
	switch (env)
	{
	case ze::PlatformEnvironnement::Editor:
		return "Editor";
	case ze::PlatformEnvironnement::Game:
		return "Game";
	}
}

}