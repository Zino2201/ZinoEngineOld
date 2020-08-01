#include "EngineCore.h"
#include "Module/ModuleManager.h"
#include "Module/Module.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "Logger/Logger.h"

namespace ZE::Module
{

std::vector<std::unique_ptr<CModule>> Modules;
OnModuleLoadedDelegate OnModuleLoaded;

void* LoadModuleHandle(const std::string& InPath)
{
#ifdef _WIN32
	// TODO: Update
	char Buffer[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, Buffer);
	std::string Path = Buffer;
	Path += "\\Binaries\\";
	Path += ZE_CONFIGURATION_NAME;
	Path += "\\";
	Path += InPath;

	return (void*) LoadLibraryA(Path.c_str());
#endif
}

void FreeModuleHandle(void* InHandle)
{
#ifdef _WIN32
	FreeLibrary((HMODULE) InHandle);
#endif
}

CModule* LoadModule(const std::string_view& InName)
{
	/**
	 * Search the module
	 */
	for(const auto& Module : Modules)
	{
		if(Module->GetName() == InName)
			return Module.get();
	}

	/** Load it */
#ifndef ZE_MONOLITHIC

	/**
	 * Load the library
	 */
	std::string Extension = ".dll";
	std::string Path = "ZinoEngine-";
	Path += InName;
	Path += Extension;
	
	void* Handle = LoadModuleHandle(Path.c_str());
	if(!Handle)
	{
		DWORD ErrMsg = GetLastError();

		LPSTR Buf = nullptr;
		size_t Size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
				| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			ErrMsg, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPSTR) &Buf, 
			0, 
			NULL);

		std::string Msg(Buf, Size);

		ZE::Logger::Error("Failed to load module {}: {}", InName,
			Msg);
		LocalFree(Buf);
		return nullptr;
	}

	CModule* Module = nullptr;

	/**
	 * Get proc address of InstantiateModule
	 */
#ifdef _WIN32
	std::string FuncName = GInstantiateModuleFuncName;
	FuncName += "_";
	FuncName += InName;
	PFN_InstantiateModuleFunc InstantiateFunc = (PFN_InstantiateModuleFunc) GetProcAddress(
		static_cast<HMODULE>(Handle), FuncName.c_str());
	Module = InstantiateFunc(); /** This indicate that your module doesn't have a DEFINE_MODULE */
	if (!Module)
	{
		ZE::Logger::Verbose("Failed to instantiate module {}", InName);
		return nullptr;
	}
	
	Module->Handle = Handle;
#endif
#else
	CModule* Module = nullptr;
	auto InstantiateFunc = CModule::InstantiateModuleFuncs[InName];
	Module = InstantiateFunc(); /** This indicate that your module doesn't have a DEFINE_MODULE */
	if (!Module)
	{
		ZE::Logger::Verbose("Failed to instantiate module {}", InName);
		return nullptr;
	}

	Module->Handle = nullptr;
#endif /** ZE_MONOLITHIC */

	Modules.emplace_back(Module);
	
	ZE::Logger::Verbose("Loaded module {}", InName);

	OnModuleLoaded.Broadcast(InName.data());

	return Module;
}

void UnloadModule(const std::string_view& InName)
{
	size_t Idx = 0;
	for (auto& Module : Modules)
	{
		if (Module->GetName() == InName)
		{
			void* Handle = Module->GetHandle();
			Modules.erase(Modules.begin() + Idx);
#ifndef ZE_MONOLITHIC
			FreeModuleHandle(Handle);
#else
			UNUSED_VARIABLE(Handle);
#endif
			break;
		}

		Idx++;
	}
}

void UnloadModules()
{
	for (size_t i = Modules.size() - 1; i > 0; --i)
	{
		CModule* Module = Modules[i].get();
		UnloadModule(Module->GetName());
	}

	Modules.clear();
}

OnModuleLoadedDelegate& GetOnModuleLoadedDelegate()
{
	return OnModuleLoaded;
}

}