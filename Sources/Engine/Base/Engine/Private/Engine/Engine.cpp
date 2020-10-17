#include "Engine/Engine.h"
#include "Module/Module.h"
#include "Module/ModuleManager.h"
#include "Engine/TickSystem.h"
#include "Profiling/Profiling.h"
#include "Console/Console.h"
#include "Render/RenderCore.h"
#include <SDL.h>
#include "Engine/InputSystem.h"
#include "Module/Module.h"
#include "AssetDatabase/AssetDatabase.h"

namespace ze
{

static ConVarRef<int32_t> CVarMaxFPS("r_maxfps", 144,
	"Max FPS.",
	1,
	GMaxFPS);

static ConVarRef<int32_t> CVarSimFPS("sim_fixed_dt", 20,
	"Fixed simulation/Physics delta time",
	1,
	60);

bool bRun = true;

ZE_DEFINE_MODULE(ze::module::DefaultModule, Engine)

/**
 * Try to load a required module
 * Crash if it fails
 */
ze::module::Module* LoadRequiredModule(const std::string_view& InName)
{
	ze::module::Module* Ptr = ze::module::load_module(InName);
	if (!Ptr)
		ze::logger::fatal("Failed to load required module {} ! Exiting", InName);

	return Ptr;
}

CZinoEngineApp::CZinoEngineApp(const bool& bInWaitForEvents) 
	: app::App(0, nullptr), bWaitForEvents(bInWaitForEvents), Now(SDL_GetPerformanceCounter()), Last(0)
{
	/** Load asset related modules */
	LoadRequiredModule("Asset");
	LoadRequiredModule("AssetDatabase");

	/** LOAD RENDERER MODULE */
	ze::logger::info("Initializing renderer");
	LoadRequiredModule("Renderer");
}

CZinoEngineApp::~CZinoEngineApp()
{
	
}

void CZinoEngineApp::process_events()
{
	SDL_Event Event;
	
	if (bWaitForEvents)
	{
		if (SDL_WaitEventTimeout(&Event, 1000))
		{
			ProcessEvent(Event);
		}
	}
	else
	{
		while (SDL_PollEvent(&Event))
		{
			ProcessEvent(Event);
		}
	}
}

void CZinoEngineApp::ProcessEvent(SDL_Event& InEvent)
{
	if (InEvent.type == SDL_QUIT)
	{
		App::exit(0);
	}

	if (InEvent.type == SDL_KEYDOWN)
		ze::input::on_key_pressed(InEvent);
	if (InEvent.type == SDL_KEYUP)
		ze::input::on_key_released(InEvent);
}

void CZinoEngineApp::loop()
{
	double DeltaTime = 0.0;

	/** Calculate delta time */
	Last = Now;
	Now = SDL_GetPerformanceCounter();
	DeltaTime = (((Now - Last) * 1000 / (double) SDL_GetPerformanceFrequency()));

	float DeltaTimeAsSec = static_cast<float>(DeltaTime * 0.001f);

	Tick(DeltaTimeAsSec);
	TickSystem.Tick(ETickFlagBits::Variable, DeltaTimeAsSec);
	TickSystem.Tick(ETickFlagBits::EndOfSimulation, DeltaTimeAsSec);
	Draw();

	/** Fps limiter */
	double SleepTime = 0.0;
	SleepTime += (1000.0 / CVarMaxFPS.get()) - DeltaTime;
	SleepTime = std::max<double>(SleepTime, 0.0);
	if (SleepTime > 0.0)
	{
		std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(SleepTime));
	}
}

} /* namespace ZE */