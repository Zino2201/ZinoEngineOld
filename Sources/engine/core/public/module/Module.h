#pragma once

#include <string>
#include "NonCopyable.h"
#include "NonMoveable.h"
#include <robin_hood.h>
#include <functional>
#include "ModuleManager.h"

namespace ze::module
{

class Module;

using InstantiateModuleFunc = Module*();
using PFN_InstantiateModuleFunc = Module*(*)();

/**
 * Module interface
 */
class Module : public NonCopyable,
	public NonMoveable
{
public:
	Module() = default;
    virtual ~Module() = default;

	/** Called after ctor. Useful for calling virtual functions since it can't be called inside the ctor. */
    virtual void initialize() {}
	
    const std::string& get_name() const { return name; }
    void set_name(const char* in_name) { name = in_name; }
    void* get_handle() const { return handle; }

#if ZE_MONOLITHIC
    /** Used for monolithic builds */
    inline static robin_hood::unordered_map<std::string_view, 
        std::function<InstantiateModuleFunc>> instantiate_module_funcs;
#endif
public:
    void* handle;
private:
    std::string name;
};

/**
 * Default module implementation
 */
class DefaultModule : public Module
{
public:
	DefaultModule() = default;
};

constexpr const char* instantiate_module_func_name = "instantiate_module";

#if ZE_MONOLITHIC
struct MonolithicRegister
{
    MonolithicRegister(const char* name, InstantiateModuleFunc func)
    {
        ze::module::Module::instantiate_module_funcs.insert({name, func});
    }
};

#define ZE_DEFINE_MODULE(Class, Name) \
    ze::module::Module* instantiate_module_##Name() \
    { \
        ze::module::Module* module = new Class;\
        module->set_name(#Name); \
        return module; \
    } \
    static ze::module::MonolithicRegister module_auto_init_##Name(#Name, &instantiate_module_##Name);
#else
#define ZE_DEFINE_MODULE(Class, Name) \
    extern "C" ZE_DLLEXPORT ze::module::Module* instantiate_module_##Name() \
    { \
        ze::module::Module* module = new Class;\
        module->set_name(#Name); \
        return module; \
    }
#endif

} /* namespace ZE */