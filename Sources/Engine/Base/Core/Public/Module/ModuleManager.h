#pragma once

#include "Logger/Logger.h"
#include <vector>
#include "Delegates/MulticastDelegate.h"

namespace ZE::Module
{

class CModule;

using OnModuleLoadedDelegate = TMulticastDelegate<const std::string_view&>;

CORE_API CModule* LoadModule(const std::string_view& InName);

template<typename T>
T* LoadModule(const std::string& InName)
{
	return reinterpret_cast<T*>(LoadModule(InName));
}

CORE_API void UnloadModule(const std::string_view& InName);
CORE_API void UnloadModules();

CORE_API OnModuleLoadedDelegate& GetOnModuleLoadedDelegate();

}