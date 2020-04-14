#pragma once

#include "Logger.h"
#include "Module/Module.h"
#include <vector>

namespace ZE
{

DECLARE_LOG_CATEGORY(ModuleManager);

class CModuleManager
{
public:
	ENGINECORE_API static CModule* LoadModule(const std::string& InName);
	ENGINECORE_API static void UnloadModule(const std::string& InName);

	template<typename T>
	static T* LoadModule(const std::string& InName)
	{
		return reinterpret_cast<T*>(LoadModule(InName));
	}

	ENGINECORE_API static void UnloadModules();
private:
	ENGINECORE_API static void* LoadDLL(const std::string& InPath);
	ENGINECORE_API static void FreeDLL(void* InHandle);
private:
	static std::vector<CModule*> Modules;
};

} /* namespace ZE */