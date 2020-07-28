#pragma once

#include "EngineCore.h"

namespace ZE::Threading
{

/**
 * Set the current thread name
 */
ENGINECORE_API void SetThreadName(const std::string_view& InStr);

/*
* Get the current thread name
*/
ENGINECORE_API std::string GetThreadName();

}