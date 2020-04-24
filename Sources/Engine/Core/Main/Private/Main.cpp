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

DECLARE_LOG_CATEGORY(EngineInit);

/**
 * ZinoEngine main
 */
int main(int argc, char** argv)
{
	CZinoEngineMain::Init();

	return 0;
}

ZE::IRenderSystem* CZinoEngineMain::RenderSystem = nullptr;
ZE::CEngine* CZinoEngineMain::Engine = nullptr;
std::thread CZinoEngineMain::RenderThreadHandle;
ZE::CRenderThread* CZinoEngineMain::RenderThread = nullptr;
ZE::CGlobalShaderCompiler* CZinoEngineMain::ShaderCompiler = nullptr;
SDL_Event CZinoEngineMain::Event;

void StartRenderThread()
{
	ZE::RenderThreadID = std::this_thread::get_id();
	ZE::GRenderThread->Run();
}

void CZinoEngineMain::PreInit()
{
	/** INITIALIZE BASE MODULES */
	{
		/** Ensure the EngineCore module is loaded */
		ZE::CModuleManager::LoadModule("EngineCore");
		ZE::CModuleManager::LoadModule("Reflection");

		ZE::GameThreadID = std::this_thread::get_id();
		SDL_InitSubSystem(SDL_INIT_VIDEO);

		/** Load core modules */
		ZE::CModuleManager::LoadModule("Engine");

		/** Load render modules */
		ZE::CModuleManager::LoadModule("RenderCore");
		ZE::CModuleManager::LoadModule("RenderSystem");
		ZE::CModuleManager::LoadModule("ShaderCompiler");
	}

	/** INITIALIZE RENDER SYSTEM **/
	{
		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing render system");
		ZE::CModuleManager::LoadModule("VulkanRenderSystem");
		ZE::CModuleManager::LoadModule("VulkanShaderCompiler");
		RenderSystem = CreateVulkanRenderSystem();
		RenderSystem->Initialize();
	}

	/** INITIALIZE BASIC SHADERS */
	{
		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing shader compiler");
		ShaderCompiler = new ZE::CGlobalShaderCompiler;

		LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing basic shaders");
	}

	/** START RENDER THREAD */
	{
		LOG(ZE::ELogSeverity::Info, EngineInit, "Starting render thread");
		RenderThread = new ZE::CRenderThread;
		RenderThreadHandle = std::thread(&StartRenderThread);
	}
}

void CZinoEngineMain::Init()
{
	CZinoEngineMain::PreInit();

	LOG(ZE::ELogSeverity::Info, EngineInit, "Initializing engine class");

	/** INITIALIZE ENGINE CLASS */
	Engine = ZE::CreateEngine();
	Engine->Initialize();

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

		DeltaTime = (((Now - Last) * 1000 / (float)SDL_GetPerformanceFrequency()))
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

	ZE::FlushRenderThread(true);

	/** Delete engine */
	delete Engine;
	Engine = nullptr;

	ZE::FlushRenderThread(true);
	RenderSystem->WaitGPU();

	/** Stopping render thread */
	RenderThread->bRun = false;
	RenderThreadHandle.join();
	delete RenderThread;

	/** Delete render system */
	RenderSystem->Destroy();
	delete RenderSystem;
	RenderSystem = nullptr;

	/** Delete shader compiler */
	delete ShaderCompiler;

	/** Clear all modules */
	ZE::CModuleManager::UnloadModules();

	SDL_Quit();
}