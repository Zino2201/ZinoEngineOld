#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include <robin_hood.h>
#include "Containers/SparseArray.h"

/**
 * Tick system
 */
namespace ze::ticksystem
{

enum class TickFlagBits : uint8_t
{
    None = 0,

    /**
     * Called at a fixed rate (synchronized with physics, independant of fps)
     */
    Fixed = 1 << 0,

    /**
     * Called at a variable rate (dependant of fps)
     */
    Variable = 1 << 1,

    /**
     * Called after Variable & Fixed (variable rate)
     */
    Late = 1 << 2
};
ENABLE_FLAG_ENUMS(TickFlagBits, TickFlags);

/** Base abstract class for tickables objects */
class ENGINE_API Tickable
{
public:
    Tickable();
    virtual ~Tickable();
    
    virtual void fixed_tick(const float in_delta_time) {}
    virtual void variable_tick(const float in_delta_time) {}
    virtual void late_tick(const float in_delta_time) {}

    ZE_FORCEINLINE TickFlags get_tick_flags() const { return tick_flags; }
    ZE_FORCEINLINE bool can_ever_tick() const { return can_tick; }
    ZE_FORCEINLINE bool is_tick_enabled() const { return enable_tick; }
protected:
    bool can_tick;
    bool enable_tick;
    TickFlags tick_flags;
};

/**
 * Queue tickable for registration
 * Registration happens in the next tick (so childs class from Tickable can modify values to control
 *  if they want to tick, and what tick flags to have)
 */
ENGINE_API void register_tickable(Tickable* in_tickable); 
ENGINE_API void unregister_tickable(Tickable* in_tickable);
ENGINE_API void tick(TickFlagBits in_flag_bit, const float in_delta_time);

}