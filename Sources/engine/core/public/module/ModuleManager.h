#pragma once

#include "logger/Logger.h"
#include <vector>
#include "delegates/MulticastDelegate.h"

namespace ze::module
{

class Module;

using OnModuleLoadedDelegate = MulticastDelegateNoRet<const std::string_view&>;

CORE_API Module* load_module(const std::string_view& InName);

template<typename T>
T* load_module(const std::string& name)
{
	return reinterpret_cast<T*>(load_module(name));
}

CORE_API void unload_module(const std::string_view& name);
CORE_API void unload_modules();

CORE_API OnModuleLoadedDelegate& get_on_module_loaded_delegate();

}