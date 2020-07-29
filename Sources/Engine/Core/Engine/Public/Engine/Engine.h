#pragma once

#include "EngineCore.h"
#include "Engine/TickSystem.h"
#include "App.h"

union SDL_Event;

namespace ZE
{

/**
 * Base abstract class for ZinoEngine apps (like editor or game)
 * Manage ticking tickable objects and fps limit
 */
class ENGINE_API CZinoEngineApp : public CApp
{
public:
    CZinoEngineApp(const bool& bInWaitForEvents);
    ~CZinoEngineApp();

    static CZinoEngineApp* Get()
    {
        return static_cast<CZinoEngineApp*>(CApp::GetCurrentApp());
    }

    void ProcessEvents() final;
    virtual void ProcessEvent(SDL_Event& InEvent);

    CTickSystem& GetTickSystem() { return TickSystem; }
protected:
    void Loop() final;
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