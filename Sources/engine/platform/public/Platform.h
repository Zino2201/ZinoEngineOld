#pragma once

#include "EngineCore.h"

namespace ze
{

class Device;

/** Operating System running the platform */
enum class PlatformOS
{
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

enum class PlatformShaderModel
{
	SM5,
	SM6,
};

enum class PlatformBackend
{
	Vulkan,
};

enum class PlatformGfxFeaturesFlagBits
{
	Raytracing = 1 << 0,
	SparseResources = 1 << 1,
	MeshShader = 1 << 2,
};
ENABLE_FLAG_ENUMS(PlatformGfxFeaturesFlagBits, PlatformGfxFeaturesFlags);

struct PlatformInfo
{
	/** Base informations */
	PlatformOS os;
	PlatformEnvironnement env;
	std::string name;
	bool is_little_endian;
	//std::vector<std::unique_ptr<Device>> devices;

	/** Rendering things */
	std::vector<PlatformShaderModel> shader_models;
	std::vector<PlatformBackend> backends;

	/** Supported gfx features (may not be supported by the backend!) */
	PlatformGfxFeaturesFlags gfx_features;

	bool is_editor() const { return env == PlatformEnvironnement::Editor; }
};

}

namespace std
{

inline std::string to_string(const ze::PlatformOS& os)
{
	switch (os)
	{
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