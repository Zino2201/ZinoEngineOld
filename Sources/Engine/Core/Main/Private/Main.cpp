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
#include "Console/Console.h"
#include "Render/RenderCore.h"
#include "Engine/TickSystem.h"

DECLARE_LOG_CATEGORY(EngineInit);

/**
 * ZinoEngine main
 */
int WinMain(int argc, char** argv)
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

static ZE::TConVar<int32_t> CVarMaxFPS("r_maxfps", 144,
	"Max FPS.",
	1,
	ZE::GMaxFPS);

static ZE::TConVar<int32_t> CVarPhysFPS("phys_fps", 5,
	"Physics FPS count.",
	1,
	60);

bool bRun = true;

void CZinoEngineMain::Loop()
{
	using namespace ZE;

	/** In sec */
	Uint64 Now = SDL_GetPerformanceCounter();
	Uint64 Last = 0;

	/** In millisec */
	double DeltaTime = 0;
	double AccumSleep = 0;

	while(bRun)
	{
		double TargetTime = 1000 / CVarMaxFPS.Get();

		/** Delta time */
		Last = Now;
		Now = SDL_GetPerformanceCounter();

		DeltaTime = (((Now - Last) * 1000 / (double) SDL_GetPerformanceFrequency()));
		float DeltaTimeSec = static_cast<float>(DeltaTime * 0.001f);

		/** Tick start of frame */
		Tick(DeltaTimeSec);
		CTickSystem::Get().Tick(ETickOrder::StartOfFrame, DeltaTimeSec);

		double PhysDeltaTime = DeltaTime;

		/** Physics */
		while(PhysDeltaTime > 0)
		{	
			double ActualDt = std::min(PhysDeltaTime, 1.0 / CVarPhysFPS.Get());
			
			CTickSystem::Get().Tick(ETickOrder::Physics, DeltaTimeSec);
		
			PhysDeltaTime -= ActualDt;
		}

		CTickSystem::Get().Tick(ETickOrder::PostPhysics, DeltaTimeSec);
		Engine->Draw();
		CTickSystem::Get().Tick(ETickOrder::EndOfFrame, DeltaTimeSec);

		
		/**
		 * Reset event at the end
		 */
		Event = {};

		/**
		 * FPS capping
		 */	
		 Uint64 EndFrameTime = SDL_GetPerformanceCounter();
		 DeltaTime = (((EndFrameTime - Now) * 1000 / (double)SDL_GetPerformanceFrequency()));
		 AccumSleep += (1000 / CVarMaxFPS.Get()) - DeltaTime;
		 if(AccumSleep > 0)
		 {
			 std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(AccumSleep));
			 Uint64 SleepTime = SDL_GetPerformanceCounter();
			 DeltaTime = (((SleepTime - EndFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency()));
			 AccumSleep -= DeltaTime;
		 }
	}

	Exit();
}

void CZinoEngineMain::Tick(const float& InDeltaTime)
{
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_QUIT)
		{
			bRun = false;
		}
	}

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