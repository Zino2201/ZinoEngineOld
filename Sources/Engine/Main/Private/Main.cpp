#include "Module/ModuleManager.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystemInterface.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "EngineCore.h"
#include "Logger/Logger.h"
#include <SDL.h>
#include "Engine/Engine.h"
#include "Shader/ShaderCompiler.h"
#include "Render/Shader/BasicShader.h"
#include "ZEFS/ZEFS.h"
#include "ZEFS/StdFileSystem.h"
#include "ZEFS/Paths.h"
#include "Threading/JobSystem/JobSystem.h"
#include <chrono>
#include "Threading/JobSystem/WorkerThread.h"
#include "Console/Console.h"
#include "Render/RenderCore.h"
#include "Engine/TickSystem.h"
#include "Engine/InputSystem.h"
#include "Gfx/Vulkan/Backend.h"

#if ZE_WITH_EDITOR
#include "Editor/ZEEditor.h"
#endif

#include "Shader/ShaderCore.h"
#include "Threading/Thread.h"
#include "Logger/Sinks/WinDbgSink.h"
#include "ZEFS/Sinks/FileSink.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>
#endif
#include <sstream>
#include <filesystem>
#include "Serialization/Types/Map.h"

namespace FS = ze::filesystem;


/** Forward decls */
int Init();
void Exit();

/** Global variables */

/** Render system ptr */
static std::unique_ptr<ze::gfx::RenderBackend> RenderBackend;

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
int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
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
#if ZE_PLATFORM(WINDOWS) && defined ZE_DEBUG
		ze::logger::add_sink(std::make_unique<ze::logger::WinDbgSink>("WinDbg"));
#endif

		/** Log file sink */
		{
			std::time_t Time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::tm* LocalTime = localtime(&Time);

			std::filesystem::create_directories("Logs/");
			
			std::stringstream ss;
			ss << std::put_time(LocalTime, "Logs/ZinoEngine_%H_%M_%S.log");
			ze::logger::add_sink(std::make_unique<FS::FileSink>("File", ss.str()));
		}
		ze::logger::info("=== ZinoEngine {} Build ===", ZE_CONFIGURATION_NAME);
		
#ifdef ZE_DEBUG
		ze::logger::warn("Using a debug build ! Except very bad performances");
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
		LoadRequiredModule("ImGui");
		LoadRequiredModule("ShaderCompiler");
	}

	/** INITIALIZE RENDER SYSTEM **/
	{
		ze::logger::info("Initializing render system");
		LoadRequiredModule("VulkanBackend");
		LoadRequiredModule("VulkanShaderCompiler");
		OwnerPtr<ze::gfx::RenderBackend> backend = ze::gfx::vulkan::create_vulkan_backend();
		auto [result, msg] = backend->initialize();
		if(!result)
			ze::logger::fatal("Failed to initialize backend:\n{}.\n\nCheck logs for additional informations.", msg);
		
		RenderBackend = std::unique_ptr<ze::gfx::RenderBackend>(backend);
	}
}

int Init()
{
	PreInit();

	ze::logger::info("Initializing and starting app");

	/** INITIALIZE ENGINE CLASS */
#if ZE_WITH_EDITOR
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

	ze::gfx::shaders::CBasicShaderManager::Get().DestroyAll();

	/** Delete render system */
	RenderBackend.reset();

	ze::jobsystem::stop();

	/** Clear all modules */
	ze::module::unload_modules();

	SDL_Quit();
}