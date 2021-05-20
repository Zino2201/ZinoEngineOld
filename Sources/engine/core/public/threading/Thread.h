#pragma once

#include "EngineCore.h"

namespace ze::threading
{

/**
 * Set the current thread name
 */
CORE_API void set_thread_name(const std::string_view& name);

/*
* Get the current thread name
*/
CORE_API std::string get_thread_name();

CORE_API std::string get_thread_name(const std::thread::id& id);

}