#include "module/ModuleManager.h"
#include "EngineCore.h"
#include "logger/Logger.h"
#include "logger/sinks/StdSink.h"
#include <SDL.h>
#include "engine/Engine.h"
#include <chrono>
#include "gfx/vulkan/Backend.h"
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
#include <sstream>
#include <filesystem>
#include "console/Console.h"
#include "serialization/types/Map.h"
#include "serialization/types/Vector.h"
#include "serialization/types/Uuid.h"
#include "serialization/Json.h"
#include "reflection/Serialization.h"
#include "assets/Asset.h"
#include "assets/AssetMetadata.h"
#include "zefs/FileStream.h"
#include "EngineVer.h"
#include "zefs/StdFileSystem.h"
#include "zefs/Paths.h"

namespace FS = ze::filesystem;


/** Forward decls */
int Init();
void Exit();

/** Global variables */

/** Render system ptr */
static std::unique_ptr<ze::gfx::Backend> RenderBackend;
static std::unique_ptr<ze::gfx::Device> Device;

/** Current app */

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
        LoadRequiredModule("Core");
        ze::threading::set_thread_name("Main Thread");

        LoadRequiredModule("ZEFS");

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

        LoadRequiredModule("Platform");
#if ZE_PLATFORM(WINDOWS)
        LoadRequiredModule("PlatformWindows");
#endif


        /** Check if required directories/files are presents */
        const std::array<const char*, 2> RequiredObjects =
                {
                        "Shaders",
                        "Assets"
                };

        for(const auto& Obj : RequiredObjects)
        {
            if(!FS::exists(Obj))
                ze::logger::fatal("Can't find directory/file \"{}\" ! Check your installation", Obj);
        }

        LoadRequiredModule("Reflection");

        /** JOB SYSTEM */
        ze::logger::info("Initializing job system");
        ze::jobsystem::initialize();

        SDL_InitSubSystem(SDL_INIT_VIDEO);

        /** Load core modules */
        LoadRequiredModule("Engine");

        /** Load render modules */
        LoadRequiredModule("ShaderCore");
        LoadRequiredModule("GfxCore");
        LoadRequiredModule("GfxBackend");
        LoadRequiredModule("Gfx");
        LoadRequiredModule("ShaderCompiler");
        LoadRequiredModule("EffectSystem");
        LoadRequiredModule("ImGui");
        LoadRequiredModule("ZEUI");
    }

    /** INITIALIZE RENDER SYSTEM **/
    {
        ze::logger::info("Initializing render system");
        LoadRequiredModule("VulkanBackend");
        LoadRequiredModule("VulkanShaderCompiler");
        OwnerPtr<ze::gfx::Backend> backend = ze::gfx::vulkan::create_vulkan_backend();
        auto [result, msg] = backend->initialize();
        if(!result)
            ze::logger::fatal("Failed to initialize backend:\n{}.\n\nCheck logs for additional informations.", msg);

        RenderBackend = std::unique_ptr<ze::gfx::Backend>(backend);
        Device = std::make_unique<ze::gfx::Device>();
    }
}

int Init()
{
    PreInit();

    ze::logger::info("Initializing and starting app");

    /** INITIALIZE ENGINE CLASS */
#if ZE_WITH_EDITOR
    LoadRequiredModule("SDFFontGen");
	LoadRequiredModule("ZEEditor");
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
    ze::module::unload_module("Renderer");

    /** Delete engine */
    engine_app.reset();

    ze::module::unload_module("EffectSystem");

    ze::gfx::Device::get().destroy();
    /** Delete render system */
    RenderBackend.reset();

    ze::jobsystem::stop();

    ze::reflection::serialization::free_archive_map();

    /** Clear all modules */
    ze::module::unload_modules();

    SDL_Quit();
}