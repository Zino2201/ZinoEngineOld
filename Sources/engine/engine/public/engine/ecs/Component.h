#pragma once

#include <type_traits>
#include "Component.gen.h"

namespace ze
{

/**
 * Base struct for all components
 */
ZSTRUCT()
struct Component 
{
	ZE_REFL_BODY()
};

template<typename T>
static constexpr bool IsComponent = std::is_base_of_v<Component, T>;

}