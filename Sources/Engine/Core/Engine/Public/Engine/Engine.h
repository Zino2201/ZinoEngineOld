#pragma once

#include "EngineCore.h"
#include "Engine/TickSystem.h"

union SDL_Event;

namespace ZE
{

class CEntityComponentSystemManager;

/**
 * Basic engine class
 * Contains system used by the whole Engine
 */
class ENGINE_API CEngine
{
public:
    virtual ~CEngine() = default;

    /** Initialize engine */
    virtual void Initialize();

    /** Tick */
    virtual void Tick(SDL_Event* InEvent, const float& InDeltaTime);

    /** Trigger frame rendering */
    virtual void Draw();

    virtual void Exit() {}
protected:
    //std::unique_ptr<CEntityComponentSystemManager> ECSManager;
};

ENGINE_API CEngine* CreateEngine();

} /* namespace ZE */