#pragma once

#include "EngineCore.h"
#include <rttr/registration>
#include <rttr/registration_friend>

/**
 * Implements RTTR functions
 */
#define REFLECTED_CLASS(...) RTTR_ENABLE(__VA_ARGS__) \
	RTTR_REGISTRATION_FRIEND

#define ZCLASS(...)
#define ZPROPERTY(...) 