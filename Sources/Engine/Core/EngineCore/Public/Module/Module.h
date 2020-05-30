#pragma once

#include <string>
#include "NonCopyable.h"
#include <unordered_map>
#include <functional>
#include "ModuleManager.h"

namespace ZE
{

typedef class CModule*(*PFN_InstantiateModule)();

/**
 * Module interface
 */
class ENGINECORE_API CModule : public CNonCopyable
{
    friend class CModuleManager;

public:
    virtual void Initialize();
    virtual void Destroy() {}
    const std::string& GetName() const { return Name; }
    void SetName(const char* InName) { Name = InName; }
    void* GetHandle() const { return Handle; }

    /** Used for monolithic builds */
    inline static std::unordered_map<std::string, std::function<CModule*()>> InstantiateModuleFuncs;
private:
    std::string Name;
    void* Handle;
};

/**
 * Default module implementation
 */
class ENGINECORE_API CDefaultModule : public CModule {};

constexpr const char* GInstantiateModuleFuncName = "InstantiateModule";

#ifdef ZE_MONOLITHIC

struct SMonolithicRegister
{
    SMonolithicRegister(const char* InName, PFN_InstantiateModule InInstantiateFunc)
    {
        CModule::InstantiateModuleFuncs.insert({InName, InInstantiateFunc});
    }
};

#define DEFINE_MODULE(Class, Name) \
    ZE::CModule* InstantiateModule_##Name() \
    { \
        ZE::CModule* Module = new Class;\
        Module->SetName(#Name); \
        return Module; \
    } \
    static ZE::SMonolithicRegister ModuleAutoInit##Name(#Name, &InstantiateModule_##Name);
#else
#define DEFINE_MODULE(Class, Name) \
    extern "C" __declspec(dllexport) ZE::CModule* InstantiateModule_##Name() \
    { \
        ZE::CModule* Module = new Class;\
        Module->SetName(#Name); \
        return Module; \
    }
#endif

} /* namespace ZE */