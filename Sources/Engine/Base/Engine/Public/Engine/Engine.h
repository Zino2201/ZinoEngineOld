#pragma once

#include "EngineCore.h"
#include "Engine/TickSystem.h"
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
protected:
    virtual void process_event(const SDL_Event& in_event);

    /**
     * Called after all ticks
     */
    virtual void post_tick(const float delta_time) {}
private:
    void loop();
private:
    bool should_run;
    bool focused;
    uint64_t now;
    uint64_t last;
    int err_code;
};

} /* namespace ze */