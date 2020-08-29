#pragma once

#include "MinimalMacros.h"
#include "Assertions.h"

#define IM_ASSERT(_EXPR) do { verify(_EXPR); } while(0)