#pragma once

#include "MinimalMacros.h"
#include "Assertions.h"

#define IM_ASSERT(_EXPR) do { ZE_ASSERT(_EXPR); } while(0)