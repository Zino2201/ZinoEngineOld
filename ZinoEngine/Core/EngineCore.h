#pragma once

/**
 * Engine core file
 * Minimal header to be included
 */

/** Types */
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <string>
#include <cstdarg>
#include <functional>
#include <optional>
#include <set>
#include <array>
#include <thread>
#include <atomic>
#include <mutex>
#include <bitset>
#include <type_traits>
#include <chrono>
#include "Core/Containers/Set.h"

/** Maths */
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include "Math.h"

/** Utilities */
#include "Core/Logger.h"
#include "Delegate.h"
#include "Threading.h"
#include <magic_enum.hpp>
#include "Flags.h"
#include "ContainerUtils.h"
#include "Reflection.h"
#include "Debugging.h"
#include "Stats/StatSystem.h"
#include "Timer.h"

/** Macros */
#define RAPIDJSON_NOMEMBERITERATORCLASS
#define DEPRECATED(Why) [[deprecated(Why)]]
#define STRINGIFY(x) #x
#define FORCEINLINE __forceinline

/** Defines */
#define SINGLETON_BODY(type) public: static type& Get(){static type Instance;return Instance;}public:type(const type&) = delete;void operator=(const type&) = delete;private:type();~type();								

extern std::thread::id GameThreadID;
extern std::thread::id RenderThreadID;
extern std::thread::id StatThreadID;

FORCEINLINE bool IsInRenderThread()
{
	return std::this_thread::get_id() == RenderThreadID;
}