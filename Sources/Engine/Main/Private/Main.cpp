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
#include "FileSystem/ZFS.h"
#include "FileSystem/StdFileSystem.h"
#include "FileSystem/FileUtils.h"
#include "Threading/JobSystem/JobSystem.h"
#include <chrono>
#include "Threading/JobSystem/WorkerThread.h"
#include "Console/Console.h"
#include "Render/RenderCore.h"
#include "Engine/TickSystem.h"
#include "Engine/InputSystem.h"
#include "Editor/ZEEditor.h"
#include "Shader/ShaderCore.h"
#include "Threading/Thread.h"
#include "Logger/Sinks/WinDbgSink.h"
#include "Logger/Sinks/FileSink.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>
#endif
#include <sstream>

namespace FS = ZE::FileSystem;


/** Forward decls */
int Init();
void Exit();

/** Global variables */

/** Render system ptr */
static std::unique_ptr<ZE::IRenderSystem> RenderSystem;

/** Current app */

/** Engine ptr */
static std::unique_ptr<ZE::CZinoEngineApp> Engine;

static ZE::TConVar<int32_t> CVarMaxFPS("r_maxfps", 144,
	"Max FPS.",
	1,
	ZE::GMaxFPS);

static ZE::TConVar<int32_t> CVarPhysFPS("phys_fps", 5,
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
	if (!ZE::Module::LoadModule(InName))
		ZE::Logger::Fatal("Failed to load required module {} ! Exiting", InName);
}

void PreInit()
{
	std::ios::sync_with_stdio(false);

	/** INITIALIZE BASE MODULES */
	{
		/** Ensure the EngineCore module is loaded */
		LoadRequiredModule("Core");
		ZE::Threading::SetThreadName("Main Thread");

		/** Mount std file system to current working dir */
		FS::IFileSystem* Root = FS::CFileSystemManager::Get()
			.AddFileSystem<ZE::FileSystem::CStdFileSystem>("Root", "/", 0,
				ZE::FileUtils::GetCurrentWorkingDirectory());
		FS::SetWriteFS(Root);

		/** Setup default sinks */
#if ZE_PLATFORM(WINDOWS) && defined ZE_DEBUG
		ZE::Logger::AddSink(std::make_unique<ZE::Logger::Sinks::CWinDbgSink>("WinDbg"));
#endif

		/** Log file sink */
		{
			std::time_t Time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::tm* LocalTime = localtime(&Time);

			std::filesystem::create_directories("Logs/");
			
			std::stringstream ss;
			ss << std::put_time(LocalTime, "Logs/ZinoEngine_%H_%M_%S.log");
			ZE::Logger::AddSink(std::make_unique<ZE::Logger::Sinks::CFileSink>("File", ss.str()));
		}
		ZE::Logger::Info("=== ZinoEngine {} Build ===", ZE_CONFIGURATION_NAME);
		
#ifdef ZE_DEBUG
		ZE::Logger::Warn("Using a debug build ! Except very bad performances");
#endif

		/** Check if required directories/files are presents */
		const std::array<const char*, 2> RequiredObjects = 
		{
			"Shaders",
			"Assets"
		};

		for(const auto& Obj : RequiredObjects)
		{
			if(!FS::Exists(Obj))
				ZE::Logger::Fatal("Can't find directory/file \"{}\" ! Check your installation", Obj);
		}

		LoadRequiredModule("Reflection");

		/** JOB SYSTEM */
		ZE::Logger::Info("Initializing job system");
		ZE::JobSystem::Initialize();

		SDL_InitSubSystem(SDL_INIT_VIDEO);

		/** Load core modules */
		LoadRequiredModule("Engine");

		/** Load render modules */
		LoadRequiredModule("ShaderCore");
		LoadRequiredModule("RenderCore");
		LoadRequiredModule("RenderSystem");
		LoadRequiredModule("ImGui");
		LoadRequiredModule("ShaderCompiler");
	}

	/** INITIALIZE RENDER SYSTEM **/
	{
		ZE::Logger::Info("Initializing render system");
		LoadRequiredModule("VulkanRenderSystem");
		LoadRequiredModule("VulkanShaderCompiler");
		RenderSystem.reset(CreateVulkanRenderSystem());
		RenderSystem->Initialize();
	}

	/** INITIALIZE BASIC SHADERS */
	{
		ZE::Logger::Info("Initializing shader compiler");
		ZE::CGlobalShaderCompiler::Get();

		ZE::Logger::Info("Initializing & compiling basic shaders");
		ZE::CBasicShaderManager::Get().CompileShaders();
	}
}

int Init()
{
	PreInit();

	ZE::Logger::Info("Initializing and starting app");

	/** INITIALIZE ENGINE CLASS */
	//Engine.reset(ZE::CreateGameApp());
	LoadRequiredModule("ZEEditor");
	Engine.reset(ZE::Editor::CreateEditor());

	/** START GAME LOOP */
	return Engine->Run();
}

void Exit()
{
	ZE::Logger::Info("Exiting engine");

	/** Unload renderer to free all renderer data */
	ZE::Module::UnloadModule("Renderer");

	/** Delete engine */
	Engine.reset();

	RenderSystem->WaitGPU();

	ZE::CBasicShaderManager::Get().DestroyAll();

	/** Delete render system */
	RenderSystem->Destroy();
	RenderSystem.reset();

	/** Clear all modules */
	ZE::Module::UnloadModules();

	SDL_Quit();
}