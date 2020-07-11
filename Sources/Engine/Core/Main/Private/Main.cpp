#include "Module/ModuleManager.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystemInterface.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "EngineCore.h"
#include "Main.h"
#include "Logger.h"
#include <SDL2/SDL.h>
#include "Engine/Engine.h"
#include "Shader/ShaderCompiler.h"
#include "Render/Shader/BasicShader.h"
#include "FileSystem/ZFS.h"
#include "FileSystem/StdFileSystem.h"
#include "FileSystem/FileUtils.h"
#include "Threading/JobSystem/JobSystem.h"
#include <chrono>
#include "Threading/JobSystem/WorkerThread.h"

DECLARE_LOG_CATEGORY(EngineInit);

/**
 * ZinoEngine main
 */
int main(int argc, char** argv)
{
	CZinoEngineMain::Init();

	return 0;
}

#include "Shader/ShaderCore.h"

namespace FS = ZE::FileSystem;

void CZinoEngineMain::PreInit()
{
	std::ios::sync_with_stdio(false);

	/** INITIALIZE BASE MODULES */
	{
		/** Ensure the EngineCore module is loaded */
		ZE::CModuleManager::LoadModule("EngineCore");
		LOG(ZE::ELogSeverity::Info, EngineInit, "=== ZinoEngine %s Build ===", ZE_CONFIGURATION_NAME);
		
#ifdef ZE_DEBUG
		LOG(ZE::ELogSeverity::Warn, EngineInit, "Using a debug build ! Except very bad performances");
#endif

		/** Mount std file system to current working dir */
		FS::IFileSystem* Root = FS::CFileSystemManager::Get()
			.AddFileSystem<ZE::FileSystem::CStdFileSystem>("Root", "/", 0,
			ZE::FileUtils::GetCurrentWorkingDirectory());
		FS::SetWriteFS(Root);

		ZE::CLogger::Get().Initialize();

		/** Check if required directories/files are presents */
		const std::array<const char*, 2> RequiredObjects = 
		{
			"/Shaders",
			"/Assets"
		};

		for(const auto& Obj : RequiredObjects)
		{
			if(!FS::Exists(Obj))
				LOG(ZE::ELogSeverity::Fatal, EngineInit, 
					"Can't find \"%s\" ! Can't continue", Obj);
		}

		ZE::CModuleManager::LoadModule("Reflection");

		/** JOB SYSTEM */
		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing job system");
		ZE::JobSystem::Initialize();

		SDL_InitSubSystem(SDL_INIT_VIDEO);

		/** Load core modules */
		ZE::CModuleManager::LoadModule("Engine");

		/** Load render modules */
		ZE::CModuleManager::LoadModule("ShaderCore");
		ZE::CModuleManager::LoadModule("RenderCore");
		ZE::CModuleManager::LoadModule("RenderSystem");
		ZE::CModuleManager::LoadModule("ImGui");
		ZE::CModuleManager::LoadModule("ShaderCompiler");
	}

	/** INITIALIZE RENDER SYSTEM **/
	{
		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing render system");
		ZE::CModuleManager::LoadModule("VulkanRenderSystem");
		ZE::CModuleManager::LoadModule("VulkanShaderCompiler");
		RenderSystem.reset(CreateVulkanRenderSystem());
		RenderSystem->Initialize();
	}

	/** INITIALIZE BASIC SHADERS */
	{
		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing shader compiler");
		ZE::CGlobalShaderCompiler::Get();

		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing & compiling basic shaders");
		ZE::CBasicShaderManager::Get().CompileShaders();
	}
}

void CZinoEngineMain::Init()
{
	CZinoEngineMain::PreInit();

	LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing engine class");

	/** INITIALIZE ENGINE CLASS */
	Engine.reset(ZE::CreateEngine());
	Engine->Initialize();

	/** LOAD RENDERER MODULE */
	LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing renderer");
	ZE::CModuleManager::LoadModule("Renderer");

	LOG(ZE::ELogSeverity::Info, EngineInit, "Starting game loop");

	/** START GAME LOOP */
	Loop();
}

void CZinoEngineMain::Loop()
{
	bool bRun = true;

	Uint64 Now = SDL_GetPerformanceCounter();
	Uint64 Last = 0;
	float DeltaTime = 0.f;

	while(bRun)
	{
		/** Delta time */
		Last = Now;
		Now = SDL_GetPerformanceCounter();

		DeltaTime = (((Now - Last) * 1000 / (double) SDL_GetPerformanceFrequency()))
			* 0.001f;

		while(SDL_PollEvent(&Event))
		{
			if(Event.type == SDL_QUIT)
			{
				bRun = false;
			}
		}

		/**
		 * Tick engine
		 */
		Tick(DeltaTime);

		/** 
		 * Execute jobs of the main thread
		 */
		ZE::JobSystem::GetWorker().Flush();

		/**
		 * Reset event at the end
		 */
		Event = {};
	}

	Exit();
}

void CZinoEngineMain::Tick(const float& InDeltaTime)
{
	Engine->Tick(&Event, InDeltaTime);
}

void CZinoEngineMain::Exit()
{
	LOG(ZE::ELogSeverity::Info, EngineInit, "Exiting engine");

	/** Delete engine */
	Engine->Exit();
	Engine.reset();

	RenderSystem->WaitGPU();

	ZE::CModuleManager::UnloadModule("Renderer");

	ZE::CBasicShaderManager::Get().DestroyAll();

	/** Delete render system */
	RenderSystem->Destroy();
	RenderSystem.reset();

	/** Clear all modules */
	ZE::CModuleManager::UnloadModules();

	SDL_Quit();
}