#pragma once

/**
 * Minimal include for classes (PCH)
 */
#include <cstdint>
#include <thread>
#include <mutex>

/** Fix for __declspec(dllexport) templates */
#pragma warning(disable: 4251)

/** Containers */
#include "Containers/Set.h"

/** Utils */
#include "Delegates/MulticastDelegate.h"


/** Maths */
#include "Maths/MathCore.h"
#include "Maths/Transform.h"
#include "Maths/Vector.h"

/** Memory */
#include "Memory/SmartPointers.h"

#include "Logger.h"

/** Macros */
#define SDL_MAIN_HANDLED
#define NOMINMAX
#define FORCEINLINE __forceinline
#ifdef _DEBUG
#define verify(condition) if(!(condition)) __debugbreak()
#define must(condition) if(!(condition)) { __debugbreak(); exit(-1); }
#else
#define verify(condition)
#define must(condition)
#endif
#define RESTRICT __restrict

namespace ZE
{

/**
 * Thread IDs
 */
ENGINECORE_API extern std::thread::id GameThreadID;
ENGINECORE_API extern std::thread::id RenderThreadID;
ENGINECORE_API extern std::thread::id StatThreadID;

FORCEINLINE bool IsInRenderThread()
{
	return std::this_thread::get_id() == RenderThreadID;
}

/** Format */
enum class EFormat
{
	Undefined,
	D32Sfloat,
	D32SfloatS8Uint,
	D24UnormS8Uint,
	R32Uint, /** uint32_t */
	R8G8B8A8UNorm, /** rgba 255 */
    B8G8R8A8UNorm, /** bgra 255 */
	R32G32Sfloat, /** vec2*/
	R32G32B32Sfloat, /** vec3 */
	R32G32B32A32Sfloat,	/** vec4 */
	R64Uint	/** uint64 */
};

enum class ESampleCount
{	
	Sample1 = 1 << 0,
	Sample2 = 1 << 1,
	Sample4 = 1 << 2,
	Sample8 = 1 << 3, 
	Sample16 = 1 << 4,
	Sample32 = 1 << 5,
	Sample64 = 1 << 6
};

/** Flags */
#include <type_traits>

#define DECLARE_FLAG_ENUM(EnumType) \
	inline EnumType operator~ (EnumType a) { return (EnumType)~(std::underlying_type<EnumType>::type)a; } \
	inline EnumType operator| (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a | (std::underlying_type<EnumType>::type)b); } \
	inline EnumType operator& (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a & (std::underlying_type<EnumType>::type)b); } \
	inline EnumType operator^ (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a ^ (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator|= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a |= (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator&= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a &= (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator^= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a ^= (std::underlying_type<EnumType>::type)b); }
#define HAS_FLAG(Enum, Other) (Enum & Other) == Other

/** Semaphore */

/**
 * A simple semaphore that can be resetted
 */
class CSemaphore
{
public:
    CSemaphore()
        : bNotified(false) {}

    void Notify()
    {
        if(!bIsWaiting)
            return;

        bNotified = true;
        Condition.notify_one();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> Lock(Mutex);
        bIsWaiting = true;
        while(!bNotified)
        {
            Condition.wait(Lock);
        }

        bNotified = false;
        bIsWaiting = false;
    }

    bool HasWaiter() const { return bIsWaiting; }
private:
    std::mutex Mutex;
    std::condition_variable Condition;
    bool bNotified;
    std::atomic_bool bIsWaiting;
};

} /* namespace ZE */

#define CString std::string