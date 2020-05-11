#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"

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

    /** Executed after physics */
    PostPhysics,

    /** Executed at the end of the frame*/
    EndOfFrame
};

/**
 * Base abstract class for objects that can be ticked
 */
class CTickable
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
class CTickSystem : public CNonCopyable
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
    std::unordered_map<ETickOrder, std::vector<CTickable*>> TickablesMap;
    std::vector<CTickable*> TickablesToAdd;
    ETickOrder CurrentOrder;
};

}