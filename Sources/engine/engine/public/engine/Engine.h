#pragma once

#include "EngineCore.h"
#include "engine/TickSystem.h"
#include "App.h"

union SDL_Event;

namespace ze
{

/**
 * Base abstract class for engine apps
 */
class ENGINE_API EngineApp
{
public:
    EngineApp();
    virtual ~EngineApp() = default;

    int run();
    void exit(int err_code);

    /**
     * Get elasped time since start of the engine
     * \return elapsed time since start in seconds
     */
    static double get_elapsed_time();
    static double get_delta_time();

    void set_focused(bool in_focused) { focused = in_focused; }
protected:
    virtual void process_event(const SDL_Event& in_event, const float in_delta_time);

    /**
     * Called after all ticks
     */
    virtual void post_tick(const float delta_time) {}
private:
    void loop();
protected:
    int32_t frame_count;
private:
    bool should_run;
    bool focused;
    std::chrono::high_resolution_clock::time_point previous;
    int err_code;
};

} /* namespace ze */