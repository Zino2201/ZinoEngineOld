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
#include <ios>
#include "FileSystem/ZFS.h"
#include "FileSystem/StdFileSystem.h"
#include "FileSystem/FileUtils.h"
#include "Serialization/FileArchive.h"

DECLARE_LOG_CATEGORY(EngineInit);

/**
 * ZinoEngine main
 */
int main(int argc, char** argv)
{
	CZinoEngineMain::Init();

	return 0;
}

void StartRenderThread()
{
	ZE::RenderThreadID = std::this_thread::get_id();
	ZE::CRenderThread::Get().Run(&CZinoEngineMain::GTSemaphore);
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
		
		/** Mount std file system to current working dir */
		FS::IFileSystem* Root = FS::CFileSystemManager::Get()
			.AddFileSystem<ZE::FileSystem::CStdFileSystem>("Root", "/", 0,
			ZE::FileUtils::GetCurrentWorkingDirectory());
		FS::SetWriteFS(Root);

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

		//{
		//	std::vector<std::string> test = { "ONE", "TWO", "THREE" };
		//	TOwnerPtr<ZE::FileSystem::IFile> File = ZE::FileSystem::Write("/Build/test.txt",
		//		ZE::FileSystem::EFileWriteFlags::ReplaceExisting);
		//	ZE::Serialization::CFileArchive FileArchive(File);
		//	FileArchive << test;
		//}

		//
		//	std::vector<std::string> test;
		//	TOwnerPtr<ZE::FileSystem::IFile> File = ZE::FileSystem::Read("/Build/test.txt");
		//	ZE::Serialization::CFileArchive FileArchive(File);
		//	FileArchive >> test;
		//

		//__debugbreak();

		ZE::CModuleManager::LoadModule("Reflection");

		ZE::GameThreadID = std::this_thread::get_id();
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

	/** START RENDER THREAD */
	{
		LOG(ZE::ELogSeverity::Info, EngineInit, "Starting render thread");
		ZE::CRenderThread::Get();
		RenderThreadHandle = std::thread(&StartRenderThread);
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
		 * Reset event at the end
		 */
		Event = {};

		ZE::FlushRenderThread(true);
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
	Engine->Exit();
	Engine.reset();

	ZE::FlushRenderThread(true);
	RenderSystem->WaitGPU();

	/** Stopping render thread */
	LOG(ZE::ELogSeverity::Info, EngineInit, "Stopping render thread");
	ZE::CRenderThread::Get().bRun = false;
	ZE::FlushRenderThread(true);
	RenderThreadHandle.join();

	ZE::CModuleManager::UnloadModule("Renderer");

	ZE::CBasicShaderManager::Get().DestroyAll();

	/** Delete render system */
	RenderSystem->Destroy();
	RenderSystem.reset();

	/** Clear all modules */
	ZE::CModuleManager::UnloadModules();

	SDL_Quit();
}