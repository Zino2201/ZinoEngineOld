#include "engine/Engine.h"
#include "module/Module.h"
#include "module/ModuleManager.h"
#include "engine/TickSystem.h"
#include "profiling/Profiling.h"
#include "console/Console.h"
#include <SDL.h>
#include "engine/InputSystem.h"
#include "module/Module.h"
#include "assetdatabase/AssetDatabase.h"

namespace ze
{

static ConVarRef<int32_t> cvar_maxfps("r_maxfps", 300,
	"Max FPS when focused. 0 to disable.",
	0,
	300);

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

EngineApp::EngineApp() : should_run(false), focused(true), frame_count(0), err_code(0)
{
	/** Load asset related modules */
	LoadRequiredModule("Asset");
	LoadRequiredModule("AssetDatabase");
	previous = std::chrono::high_resolution_clock::now();
}

void EngineApp::process_event(const SDL_Event& in_event, const float in_delta_time)
{
	if (in_event.type == SDL_QUIT)
		exit(0);

	if (in_event.type == SDL_KEYDOWN)
		ze::input::on_key_pressed(in_event);
	if (in_event.type == SDL_KEYUP)
		ze::input::on_key_released(in_event);
	if (in_event.type == SDL_MOUSEMOTION)
		ze::input::set_mouse_delta(maths::Vector2f(in_event.motion.xrel, in_event.motion.yrel));

	/*
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
	*/
}

int EngineApp::run()
{
	should_run = true;
	focused = true;

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

double engine_elapsed_time = 0.0;
double engine_delta_time = 0.0;

void EngineApp::loop()
{
	auto current = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> delta_time = current - previous;
	previous = current;

	engine_delta_time = delta_time.count();
	float delta_time_as_secs = static_cast<float>(engine_delta_time) * 0.001f;

	engine_elapsed_time += delta_time_as_secs;

	ze::input::clear();

	/** Process events */
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			process_event(event, delta_time_as_secs);
		}
	}

	ticksystem::tick(ticksystem::TickFlagBits::Variable, delta_time_as_secs);
	ticksystem::tick(ticksystem::TickFlagBits::Late, delta_time_as_secs);
	post_tick(delta_time_as_secs);

	/** Fps limiter */
	if(cvar_maxfps.get() != 0)
	{
		using namespace std::chrono_literals;

		focused = true;
		const std::chrono::duration<double, std::milli> min_ms(focused ? (1000.0 / cvar_maxfps.get()) : (1000.0 / cvar_unfocus_fps.get()));
		const auto target_sleep_time = current + min_ms;
		std::this_thread::sleep_until(target_sleep_time - 1ms);
		while(std::chrono::high_resolution_clock::now() < target_sleep_time) {}
	}

	frame_count++;
}

double EngineApp::get_elapsed_time()
{
	return engine_elapsed_time;
}

double EngineApp::get_delta_time()
{
	return engine_delta_time;
}

} /* namespace ze */