#include "EngineCore.h"
#include "Module/ModuleManager.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "Logger.h"

namespace ZE
{

std::vector<CModule*> CModuleManager::Modules;

void CModule::Initialize() {}

CModule* CModuleManager::LoadModule(const std::string& InName)
{
	/**
	 * Search the module
	 */
	for(const auto& Module : Modules)
	{
		if(Module->GetName() == InName)
			return Module;
	}

	/**
	 * Load the module
	 */

	/**
	 * Load the library
	 */
	std::string Extension = ".dll";
	std::string Path = "ZinoEngine-" + InName + Extension;
	void* Handle = LoadDLL(Path.c_str());
	if(!Handle)
	{
		LOG(ELogSeverity::Error, ModuleManager, "Failed to load module %s", InName.c_str());
		return nullptr;
	}

	CModule* Module = nullptr;

	/**
	 * Get proc address of InstantiateModule
	 */
#ifdef _WIN32
	PFN_InstantiateModule InstantiateFunc = (PFN_InstantiateModule) GetProcAddress(
		static_cast<HMODULE>(Handle), GInstantiateModuleFuncName);
	Module = InstantiateFunc(); /** This indicate that your module doesn't have a DEFINE_MODULE */
	if (!Module)
	{
		LOG(ELogSeverity::Error, ModuleManager, "Failed to instantiate module %s", InName.c_str());
		return nullptr;
	}
	
	Module->Handle = Handle;
#endif

	Modules.push_back(Module);
	
	LOG(ELogSeverity::Info, ModuleManager, "Loaded module %s", InName.c_str());

	Module->Initialize();

	OnModuleLoaded.Broadcast(InName.c_str());

	return Module;
}

void* CModuleManager::LoadDLL(const std::string& InPath)
{
#ifdef _WIN32
	return (void*) LoadLibraryA(InPath.c_str());
#endif
}

void CModuleManager::FreeDLL(void* InHandle)
{
#ifdef _WIN32
	FreeLibrary((HMODULE) InHandle);
#endif
}

void CModuleManager::UnloadModules()
{
	for (size_t i = Modules.size() - 1; i > 0; --i)
	{
		CModule* Module = Modules[i];
		UnloadModule(Module->GetName());
	}

	Modules.clear();
}

void CModuleManager::UnloadModule(const std::string& InName)
{
	size_t Idx = 0;
	for (auto& Module : Modules)
	{
		if(Module->GetName() == InName)
		{
			Module->Destroy();
			FreeDLL(Module->GetHandle());
			delete Module;
			Modules.erase(Modules.begin() + Idx);
			break;
		}

		Idx++;
	}
}

} /* namespace ZE */