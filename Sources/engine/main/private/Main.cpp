#include "module/ModuleManager.h"
#include "EngineCore.h"
#include "logger/Logger.h"
#include "logger/sinks/StdSink.h"
#include <SDL.h>
#include "engine/Engine.h"
#include <chrono>
#include "gfx/Gfx.h"
#if ZE_WITH_EDITOR
#include "editor/ZEEditor.h"
#else
#include "engine/EngineGame.h"
#endif
#include "threading/Thread.h"
#include "threading/jobsystem/JobSystem.h"
#include "logger/sinks/WinDbgSink.h"
#include "zefs/sinks/FileSink.h"
#include "zefs/ZEFS.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>
#endif
#include <filesystem>
#include "console/Console.h"
#include "serialization/Json.h"
#include "reflection/Serialization.h"
#include "assets/Asset.h"
#include "zefs/StdFileSystem.h"
#include "zefs/Paths.h"
#include "gfx/effect/EffectDatabase.h"
#include "PlatformMgr.h"
#include "gfx/BackendManager.h"

namespace FS = ze::filesystem;


/** Forward decls */
int Init();
void Exit();

/** Global variables */
std::unique_ptr<ze::gfx::Device> gfx_device;

/** Engine ptr */
static std::unique_ptr<ze::EngineApp> engine_app;

static ze::ConVarRef<int32_t> CVarPhysFPS("phys_fps", 5,
                                          "Physics FPS count.",
                                          1,
                                          60);

bool bRun = true;

/**
 * ZinoEngine main
 */
#if ZE_PLATFORM(WINDOWS)

int main(int argc, char** argv)
{
	char Buffer[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, Buffer);
	std::string Path = Buffer;
	Path += "\\Binaries\\";
	Path += ZE_CONFIGURATION_NAME;
	SetDllDirectoryA(Path.c_str());


	int Err = Init();
	Exit();
	return Err;
}

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	return main(0, nullptr);
}

#else
int main(int argc, char** argv)
{
    int Err = Init();
    Exit();
    return Err;
}
#endif

/**
 * Try to load a required module
 * Crash if it fails
 */
void LoadRequiredModule(const std::string_view& InName)
{
    if (!ze::module::load_module(InName))
        ze::logger::fatal("Failed to load required module {} ! Exiting", InName);
}

void PreInit()
{
    std::ios::sync_with_stdio(false);

    /** INITIALIZE BASE MODULES */
    {
        /** Ensure the EngineCore module is loaded */
        LoadRequiredModule("core");
        ze::threading::set_thread_name("Main Thread");

        LoadRequiredModule("zefs");

        /** Mount std file system to current working dir */
        FS::FileSystem& Root = FS::add_filesystem<FS::StdFileSystem>("Root", "/", 0,
                                                                     FS::get_current_working_dir());
        FS::set_write_fs(Root);

        /** Setup default sinks */
#if ZE_FEATURE(DEVELOPMENT)
        ze::logger::add_sink(std::make_unique<ze::logger::StdSink>("Std"));
    #if ZE_PLATFORM(WINDOWS)
		ze::logger::add_sink(std::make_unique<ze::logger::WinDbgSink>("WinDbg"));
    #endif /** ZE_PLATFORM(WINDOWS) */
#endif /** ZE_DEBUG */

        /** Log file sink */
        {
            std::time_t Time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* LocalTime = localtime(&Time);

            std::filesystem::create_directories("Logs/");

            ze::logger::add_sink(std::make_unique<FS::FileSink>("File", "Logs/Latest.log"));
        }
        ze::logger::info("=== ZinoEngine {} Build ===", ZE_CONFIGURATION_NAME);

#ifdef ZE_DEBUG
        ze::logger::warn("Using a debug build ! Except very bad performances");
#endif

        LoadRequiredModule("platform");

    	/** Find installed platforms */
    	ze::find_platforms();

        /** Check if required directories/files are presents */
        const std::array<const char*, 3> RequiredObjects =
        {
			"Assets",
			"Config",
			"Shaders"
        };

        for(const auto& Obj : RequiredObjects)
        {
            if(!FS::exists(Obj))
                ze::logger::fatal("Can't find directory/file \"{}\" ! Check your installation", Obj);
        }

        LoadRequiredModule("reflection");

        /** JOB SYSTEM */
        ze::logger::info("Initializing job system");
        ze::jobsystem::initialize();

        SDL_InitSubSystem(SDL_INIT_VIDEO);

        /** Load core modules */
        LoadRequiredModule("engine");

        /** Load render modules */
        LoadRequiredModule("shadercore");
        LoadRequiredModule("gfxbackend");
        LoadRequiredModule("gfx");
        LoadRequiredModule("shadercompiler");
        LoadRequiredModule("effect");
        LoadRequiredModule("imgui");
    }

    /** INITIALIZE RENDER SYSTEM **/
    {
        ze::logger::info("Initializing render system");
        ze::gfx::find_backends();

    	/** For now only uses Vulkan */
        bool success = false;
    	for(const auto& backend : ze::gfx::get_backends())
    	{
    		if(backend.name == "Vulkan")
                success = create_backend(&backend, ze::gfx::BackendShaderModel::ShaderModel6_0);
    	}

    	if(!success)
            ze::logger::fatal("Failed to load a graphics backend!");

    	gfx_device = std::make_unique<ze::gfx::Device>();
    }
}

int Init()
{
    PreInit();

    ze::logger::info("Initializing and starting app");

    /** INITIALIZE ENGINE CLASS */
#if ZE_WITH_EDITOR
	LoadRequiredModule("editor");
	engine_app = std::make_unique<ze::editor::EditorApp>();
#else
    engine_app = std::make_unique<ze::GameApp>();
#endif

    /** START GAME LOOP */
    return engine_app->run();
}

void Exit()
{
    ze::logger::info("Exiting engine");

    /** Unload renderer to free all renderer data */
    ze::module::unload_module("renderer");

    /** Delete engine */
    engine_app.reset();

    ze::module::unload_module("effect");

    ze::gfx::Device::get().destroy();

    /** Delete render system */
    gfx_device.reset();
	ze::gfx::destroy_running_backend();

    ze::jobsystem::stop();

    ze::reflection::serialization::free_archive_map();

    /** Clear all modules */
    ze::module::unload_modules();

    SDL_Quit();
}