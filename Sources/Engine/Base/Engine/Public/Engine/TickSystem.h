#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include <robin_hood.h>
#include "Containers/CoherentArray.h"

/**
 * Tick system
 */
namespace ze
{

/**
 * Tick group
 */
enum class ETickFlagBits
{
    None = 0,

    /**
    * First order called in the game loop
    * Fixed tick group, this is called at fixed time amount and is preferred for physics
    * interaction as a physics engine work better at a fixed timestep. 
    * NOTE: This group is called before actual physics simulation */
    Fixed = 1 << 0,

    /** Variable tick group, that is frame-rate dependent, use the provided delta time to
    * ensure consistency over frames. Do not interact with physics using this group ! Prefer Fixed */
    Variable = 1 << 1,

    /**
     * Called before rendering and after fixed & variable tick
     */
    EndOfSimulation = 1 << 2,
};
ENABLE_FLAG_ENUMS(ETickFlagBits, ETickFlags);

/**
 * Base abstract class for objects that can be ticked
 * By default, a tickable object is set to tick at a variable rate, and ticking is enabled by default
 */
class ENGINE_API CTickable
{
    friend class CTickSystem;

public:
    CTickable();
    virtual ~CTickable();

    /**
     * Variable tick
     */
    virtual void Tick(const float& InDeltaTime) {}

    /**
     * Used for Fixed ticking
     */
    virtual void FixedTick(const float& InDeltaTime) {}

    /**
     * EndOfSimulation
     */
    virtual void LateTick(const float& InDeltaTime) {}

    const bool& CanEverTick() const { return bCanEverTick; }
    const bool& IsTickEnabled() const { return bIsTickEnabled; }
    const ETickFlags& GetTickFlags() const { return TickFlags; }
protected:
    bool bCanEverTick;
    bool bIsTickEnabled;
    ETickFlags TickFlags;
private:
    size_t TickableIdx;
};

/**
 * Engine tick system
 */
class ENGINE_API CTickSystem
{
public:
    CTickSystem() = default;

    void Tick(ETickFlagBits InFlag, const float& InDeltaTime);
    void Register(CTickable& InTickable);
    void Unregister(CTickable& InTickable);

    ETickFlagBits GetCurrentTick() const { return CurrentTick; }
private:
    robin_hood::unordered_map<ETickFlagBits, TCoherentArray<CTickable*>> TickablesMap;
    std::vector<CTickable*> TickablesToAdd;
    ETickFlagBits CurrentTick;
};

}