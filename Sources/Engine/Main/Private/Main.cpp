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
static std::unique_ptr<ze::IRenderSystem> RenderSystem;

/** Current app */

/** Engine ptr */
static std::unique_ptr<ze::CZinoEngineApp> Engine;

static ze::ConVarRef<int32_t> CVarMaxFPS("r_maxfps", 144,
	"Max FPS.",
	1,
	ze::GMaxFPS);

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
		LoadRequiredModule("RenderCore");
		LoadRequiredModule("RenderSystem");
		LoadRequiredModule("ImGui");
		LoadRequiredModule("ShaderCompiler");
	}

	/** INITIALIZE RENDER SYSTEM **/
	{
		ze::logger::info("Initializing render system");
		LoadRequiredModule("VulkanRenderSystem");
		LoadRequiredModule("VulkanShaderCompiler");
		RenderSystem.reset(CreateVulkanRenderSystem());
		RenderSystem->Initialize();
	}

	/** INITIALIZE BASIC SHADERS */
	{
		ze::logger::info("Initializing & compiling basic shaders");
		ze::gfx::shaders::CBasicShaderManager::Get().CompileShaders();
	}
}

#include "Reflection/Registration.h"
#include "Reflection/Class.h"
#include "Reflection/Builders.h"
#include "Reflection/Traits.h"
#include "Reflection/Cast.h"
#include "Reflection/Any.h"
#include "Reflection/Macros.h"

class Parent
{
public:
	int32_t A = 2000;
	std::string cava = "oui oui";

	Parent() : A (5000)
	{
		
	}

	ZE_REFL_DECLARE_CLASS_BODY(Parent)
};

class Child : public Parent
{
public:
	int8_t B = 22;

	ZE_REFL_DECLARE_CLASS_BODY(Child)
};

ZE_REFL_DECLARE_CLASS(Parent)
ZE_REFL_DECLARE_CLASS(Child)

int Init()
{
	PreInit();

	ze::reflection::builders::ClassBuilder<Parent>("Parent")
		.property<int32_t>("A", &Parent::A);
	ze::reflection::builders::ClassBuilder<Child>("Child")
		.constructor()
		.parent("Parent")
		.property<int8_t>("B", &Child::B);

	int sz = sizeof(Parent);

	Parent pp;
	pp.A = 121;
	const ze::reflection::Property* prop = ze::reflection::Class::get<Parent>()->get_property("A");
	prop->set_value(&pp, 1201);
	const auto& val = prop->get_value(&pp);
	const auto& real_val = val.get_value<int32_t>();

	Parent* a = new Child;
	Parent* b = new Parent;

	// instantiate
	Child* test_inst = ze::reflection::Class::get<Child>()->instantiate<Child>();

	Child* legal = ze::reflection::cast<Child>(a);
	Child* illegal = ze::reflection::cast<Child>(b);
	
	ZE_DEBUGBREAK();

	ze::logger::info("Initializing and starting app");

	/** INITIALIZE ENGINE CLASS */
#if ZE_WITH_EDITOR
	LoadRequiredModule("ZEEditor");
	Engine.reset(ze::editor::CreateEditor());
#else
	Engine.reset(ze::CreateGameApp());
#endif

	/** START GAME LOOP */
	return Engine->run();
}

void Exit()
{
	ze::logger::info("Exiting engine");

	/** Unload renderer to free all renderer data */
	ze::module::unload_module("Renderer");

	/** Delete engine */
	Engine.reset();

	RenderSystem->WaitGPU();

	ze::gfx::shaders::CBasicShaderManager::Get().DestroyAll();

	/** Delete render system */
	RenderSystem->Destroy();
	RenderSystem.reset();

	/** Clear all modules */
	ze::module::unload_modules();

	SDL_Quit();
}