#include "EngineCore.h"
#include "Module/ModuleManager.h"
#include "Module/Module.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
#include <dlfcn.h>
#include <unistd.h>
#endif
#include "Logger/Logger.h"

namespace ZE::Module
{

std::vector<std::unique_ptr<CModule>> Modules;
OnModuleLoadedDelegate OnModuleLoaded;

void* LoadModuleHandle(const std::string& InPath)
{
#if ZE_PLATFORM(WINDOWS)
	// TODO: Update
	char Buffer[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, Buffer);
	std::string Path = Buffer;
	Path += "\\Binaries\\";
	Path += ZE_CONFIGURATION_NAME;
	Path += "\\";
	Path += InPath;

	return (void*) LoadLibraryA(Path.c_str());
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
	//char Buffer[PATH_MAX];
	//getcwd(Buffer, PATH_MAX);

	std::string Path = InPath;
	//Path += "/Binaries/";
	//Path += ZE_CONFIGURATION_NAME;
	//Path += "/";
	//Path += InPath;

	return dlopen(Path.c_str(), RTLD_LAZY);
#endif
}

void FreeModuleHandle(void* InHandle)
{
#if ZE_PLATFORM(WINDOWS)
	FreeLibrary((HMODULE) InHandle);
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
	dlclose(InHandle);
#endif 
}

/** Platform specific variables */
#if ZE_PLATFORM(WINDOWS)
constexpr static std::string_view GetSharedLibExtension() { return "dll"; }
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
constexpr static std::string_view GetSharedLibExtension() { return "so"; }
#endif

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
	CModule* Module = nullptr;

#if ZE_PLATFORM(WINDOWS)
	std::string Path = "ZinoEngine-";
	Path += InName;
	Path += ".";
	Path += GetSharedLibExtension();

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

	/**
	 * Get proc address of InstantiateModule
	 */
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
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
	std::string Path = "lib";
	Path += InName;
	Path += ".";
	Path += GetSharedLibExtension();

	void* Handle = LoadModuleHandle(Path.c_str());
	if(!Handle)
	{
		ZE::Logger::Fatal("Failed to load module {}", InName);
	}

	/**
	 * Get proc address of InstantiateModule
	 */
	std::string FuncName = GInstantiateModuleFuncName;
	FuncName += "_";
	FuncName += InName;
	PFN_InstantiateModuleFunc InstantiateFunc = (PFN_InstantiateModuleFunc) dlsym(Handle,
		FuncName.c_str());
	Module = InstantiateFunc();
	if (!Module)
	{
		ZE::Logger::Verbose("Failed to instantiate module {}", InName);
		return nullptr;
	}

	Module->Handle = Handle;
#endif /** ZE_PLATFORM(WINDOWS) */

#else /** ZE_MONOLITHIC */
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