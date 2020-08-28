#pragma once

#include "EngineCore.h"

namespace ZE::Threading
{

/**
 * Set the current thread name
 */
CORE_API void SetThreadName(const std::string_view& InStr);

/*
* Get the current thread name
*/
CORE_API std::string GetThreadName();

CORE_API std::string GetThreadName(const std::thread::id& InID);

}