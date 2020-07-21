#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include <robin_hood.h>

/**
 * Tick system
 */
namespace ZE
{

/**
 * When to tick
 */
enum class ETickOrder
{
    /** Executed for each order called, 
     *   you can get the current order with CTickSystem::GetCurrentOrder */
    All,

    /** Executed at the beginning of the frame and before physics */
    StartOfFrame,

    /** For physics only */
    Physics,

    /** Executed after physics */
    PostPhysics,

    /** Executed at the end of the frame after rendering has been queued */
    EndOfFrame
};

/**
 * Base abstract class for objects that can be ticked
 */
class ENGINE_API CTickable
{
public:
    CTickable();
    virtual ~CTickable();

    virtual void Tick(const float& InDeltaTime) = 0;
    virtual ETickOrder GetTickOrder() const = 0;
};

/**
 * Engine tick system
 */
class ENGINE_API CTickSystem : public CNonCopyable
{
public:
    static CTickSystem& Get()
    {
        static CTickSystem Instance;
        return Instance;
    }

    void Tick(ETickOrder InOrder, const float& InDeltaTime);
    void Register(CTickable& InTickable);
    void Unregister(CTickable& InTickable);

    ETickOrder GetCurrentOrder() const { return CurrentOrder; }
private:
    CTickSystem() = default;
private:
    robin_hood::unordered_map<ETickOrder, std::vector<CTickable*>> TickablesMap;
    std::vector<CTickable*> TickablesToAdd;
    ETickOrder CurrentOrder;
};

}