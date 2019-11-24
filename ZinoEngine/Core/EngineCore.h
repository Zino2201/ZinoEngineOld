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

/** Utilities */
#include "Core/Logger.h"

/** Defines */
#define SINGLETON_BODY(type) public: static type& Get(){static type Instance;return Instance;}public:type(const type&) = delete;void operator=(const type&) = delete;private:type();~type();								