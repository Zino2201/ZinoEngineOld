#pragma once

#include "EngineCore.h"
#include "Engine/TickSystem.h"
#include "App.h"

union SDL_Event;

namespace ze
{

/**
 * Base abstract class for ZinoEngine apps (like editor or game)
 * Manage ticking tickable objects and fps limit
 */
class ENGINE_API CZinoEngineApp : public app::App
{
public:
    CZinoEngineApp(const bool& bInWaitForEvents);
    ~CZinoEngineApp();

    static CZinoEngineApp* Get()
    {
        return static_cast<CZinoEngineApp*>(app::App::get());
    }

    void process_events() final;
    virtual void ProcessEvent(SDL_Event& InEvent);

    CTickSystem& GetTickSystem() { return TickSystem; }
protected:
    void loop() final;
    virtual void Tick(const float& InDeltaTime) = 0;
    virtual void Draw() = 0;
protected:
    bool bWaitForEvents;
private:
    CTickSystem TickSystem;
    uint64_t Now;
    uint64_t Last;
};

ENGINE_API CZinoEngineApp* CreateGameApp();

} /* namespace ZE */