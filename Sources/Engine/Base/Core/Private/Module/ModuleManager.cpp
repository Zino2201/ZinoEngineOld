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

namespace ze::module
{

std::vector<std::unique_ptr<Module>> modules;
OnModuleLoadedDelegate on_module_loaded;

void* load_module_handle(const std::string& path)
{
#if ZE_PLATFORM(WINDOWS)
	return (void*) LoadLibraryA(path.c_str());
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
	return dlopen(path.c_str(), RTLD_LAZY);
#endif
}

void free_module_handle(void* handle)
{
#if ZE_PLATFORM(WINDOWS)
	FreeLibrary((HMODULE) handle);
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
	dlclose(handle);
#endif 
}

/** Platform specific variables */
#if ZE_PLATFORM(WINDOWS)
constexpr static std::string_view get_shared_lib_extension() { return "dll"; }
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
constexpr static std::string_view get_shared_lib_extension() { return "so"; }
#endif

Module* load_module(const std::string_view& name)
{
	/**
	 * Search the module
	 */
	for(const auto& module : modules)
	{
		if(module->get_name() == name)
			return module.get();
	}

	/** Load it */
#if !ZE_MONOLITHIC

	/**
	 * Load the library
	 */
	Module* module = nullptr;

#if ZE_PLATFORM(WINDOWS)
	std::string path = std::string(name);
	path += ".";
	path += get_shared_lib_extension();

	void* handle = load_module_handle(path.c_str());
	if(!handle)
	{
		DWORD err_msg = GetLastError();

		LPSTR buf = nullptr;
		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
				| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			err_msg, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPSTR) &buf, 
			0, 
			NULL);

		std::string msg(buf, size);
		ze::logger::error("Failed to load module {}: {}", name,
			msg);
		LocalFree(buf);
		return nullptr;
	}

	/**
	 * Get proc address of InstantiateModule
	 */
	std::string func_name = instantiate_module_func_name;
	func_name += "_";
	func_name += name;
	PFN_InstantiateModuleFunc func = (PFN_InstantiateModuleFunc) GetProcAddress(
		static_cast<HMODULE>(handle), func_name.c_str());
	if (!func)
		ze::logger::fatal("Module {} isn't a valid Module ! (missing or incorrect DEFINE_MODULE)", name);
	module = func();
	if (!module)
	{
		ze::logger::verbose("Failed to instantiate module {}", name);
		return nullptr;
	}
	
	module->handle = handle;
#elif ZE_PLATFORM(OSX) || ZE_PLATFORM(LINUX)
	std::string path = "lib";
	path += name;
	path += ".";
	path += get_shared_lib_extension();

	void* handle = load_module_handle(path.c_str());
	if(!handle)
	{
		ze::logger::fatal("Failed to load module {}", name);
	}

	/**
	 * Get proc address of InstantiateModule
	 */
	std::string func_name = instantiate_module_func_name;
	func_name += "_";
	func_name += name;
	PFN_InstantiateModuleFunc func = (PFN_InstantiateModuleFunc) dlsym(handle,
		func_name.c_str());
	module = func();
	if (!module)
	{
		ze::logger::verbose("Failed to instantiate module {}", name);
		return nullptr;
	}

	module->handle = handle;
#endif /** ZE_PLATFORM(WINDOWS) */

#else /** ZE_MONOLITHIC */
	CModule* Module = nullptr;
	auto InstantiateFunc = CModule::InstantiateModuleFuncs[InName];
	if(!InstantiateFunc)
		ze::logger::fatal("Module {} isn't a valid Module ! (missing or incorrect DEFINE_MODULE)", InName);

	Module = InstantiateFunc();
	if (!Module)
	{
		ZE::Logger::Verbose("Failed to instantiate module {}", InName);
		return nullptr;
	}

	Module->Handle = nullptr;
#endif /** ZE_MONOLITHIC */

	modules.emplace_back(module);
	
	ze::logger::verbose("Loaded module {}", name);

	on_module_loaded.broadcast(name.data());

	return module;
}

void unload_module(const std::string_view& name)
{
	size_t idx = 0;
	for (auto& module : modules)
	{
		if (module->get_name() == name)
		{
			modules.erase(modules.begin() + idx);
#if !ZE_MONOLITHIC
			void* handle = module->get_handle();
			free_module_handle(handle);
#endif
			break;
		}

		idx++;
	}
}

void unload_modules()
{
	for (size_t i = modules.size() - 1; i > 0; --i)
	{
		Module* module = modules[i].get();
		unload_module(module->get_name());
	}

	modules.clear();
}

OnModuleLoadedDelegate& get_on_module_loaded_delegate()
{
	return on_module_loaded;
}

}