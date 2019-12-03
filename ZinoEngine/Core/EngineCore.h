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

/** Maths */
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/** Utilities */
#include "Core/Logger.h"
#include "Delegate.h"
#include "Threading.h"
#include <magic_enum.hpp>
#include "Flags.h"
#include "ContainerUtils.h"

/** Macros */
#define RAPIDJSON_NOMEMBERITERATORCLASS

/** Defines */
#define SINGLETON_BODY(type) public: static type& Get(){static type Instance;return Instance;}public:type(const type&) = delete;void operator=(const type&) = delete;private:type();~type();								