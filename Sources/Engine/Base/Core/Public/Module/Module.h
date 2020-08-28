#pragma once

#include <string>
#include "NonCopyable.h"
#include <robin_hood.h>
#include <functional>
#include "ModuleManager.h"

namespace ZE::Module
{

using InstantiateModuleFunc = CModule*();
using PFN_InstantiateModuleFunc = CModule*(*)();

/**
 * Module interface
 */
class CORE_API CModule : public CNonCopyable
{
public:
    virtual ~CModule() = default;

    const std::string& GetName() const { return Name; }
    void SetName(const char* InName) { Name = InName; }
    void* GetHandle() const { return Handle; }

    /** Used for monolithic builds */
    inline static robin_hood::unordered_map<std::string_view, 
        std::function<InstantiateModuleFunc>> InstantiateModuleFuncs;
public:
    void* Handle;
private:
    std::string Name;
};

/**
 * Default module implementation
 */
class CORE_API CDefaultModule : public CModule {};

constexpr const char* GInstantiateModuleFuncName = "InstantiateModule";

#ifdef ZE_MONOLITHIC

struct SMonolithicRegister
{
    SMonolithicRegister(const char* InName, InstantiateModuleFunc InInstantiateFunc)
    {
        CModule::InstantiateModuleFuncs.insert({InName, InInstantiateFunc});
    }
};

#define DEFINE_MODULE(Class, Name) \
    ZE::Module::CModule* InstantiateModule_##Name() \
    { \
        ZE::Module::CModule* Module = new Class;\
        Module->SetName(#Name); \
        return Module; \
    } \
    static ZE::Module::SMonolithicRegister ModuleAutoInit##Name(#Name, &InstantiateModule_##Name);
#else
#define DEFINE_MODULE(Class, Name) \
    extern "C" ZE_DLLEXPORT ZE::Module::CModule* InstantiateModule_##Name() \
    { \
        ZE::Module::CModule* Module = new Class;\
        Module->SetName(#Name); \
        return Module; \
    }
#endif

} /* namespace ZE */