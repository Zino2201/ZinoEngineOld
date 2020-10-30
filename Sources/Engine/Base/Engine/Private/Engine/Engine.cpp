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

static ConVarRef<int32_t> cvar_maxfps("r_maxfps", 144,
	"Max FPS.",
	1,
	GMaxFPS);

static ConVarRef<int32_t> cvar_unfocus_fps("r_unfocusfps", 15,
	"Max FPS when unfocused",
	1,
	1000);

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

EngineApp::EngineApp() : should_run(false), focused(true), now(0), last(0), err_code(0)
{
	/** Load asset related modules */
	LoadRequiredModule("Asset");
	LoadRequiredModule("AssetDatabase");

	/** LOAD RENDERER MODULE */
	//ze::logger::info("Initializing renderer");
	//LoadRequiredModule("Renderer");
}

void EngineApp::process_event(const SDL_Event& in_event)
{
	if (in_event.type == SDL_QUIT)
		exit(0);

	if (in_event.type == SDL_KEYDOWN)
		ze::input::on_key_pressed(in_event);
	if (in_event.type == SDL_KEYUP)
		ze::input::on_key_released(in_event);
	
	if (in_event.type == SDL_WINDOWEVENT)
	{
		switch(in_event.window.event)
		{
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			focused = true;
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			focused = false;
			break;
		}
	}
}

int EngineApp::run()
{
	should_run = true;

	while(should_run)
	{
		loop();
	}

	return err_code;
}

void EngineApp::exit(int in_err_code)
{
	should_run = false;
	err_code = in_err_code;
}

void EngineApp::loop()
{
	double delta_time = 0.0;
	last = now;
	now = SDL_GetPerformanceCounter();
	delta_time = (((now - last) * 1000 / (double) SDL_GetPerformanceFrequency()));

	float delta_time_as_secs = static_cast<float>(delta_time * 0.001f);

	/** Process events */
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			process_event(event);
		}
	}

	ticksystem::tick(ticksystem::TickFlagBits::Variable, delta_time_as_secs);
	ticksystem::tick(ticksystem::TickFlagBits::Late, delta_time_as_secs);
	post_tick(delta_time_as_secs);

	/** Fps limiter */
	const double max_ms = focused ? (1000.0 / cvar_maxfps.get()) : (1000.0 / cvar_unfocus_fps.get());
	double sleep_time = std::max(max_ms - delta_time, 0.0);
	if (sleep_time > 0.0)
	{
		std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(sleep_time));
	}
}

} /* namespace ze */