#pragma once

#include <string>

namespace ZE
{

/**
 * Module interface
 */
class ENGINECORE_API CModule
{
    friend class CModuleManager;

public:
    virtual void Initialize();
    const char* GetName() const { return Name; }
    void SetName(const char* InName) { Name = InName; }
    void* GetHandle() const { return Handle; }
private:
    const char* Name;
    void* Handle;
};

/**
 * Default module implementation
 */
class ENGINECORE_API CDefaultModule : public CModule {};

constexpr const char* GInstantiateModuleFuncName = "InstantiateModule";
typedef CModule*(*PFN_InstantiateModule)();

#define DEFINE_MODULE(Class, Name) \
    extern "C" __declspec(dllexport) ZE::CModule* InstantiateModule() \
    { \
        ZE::CModule* Module = new Class;\
        Module->SetName(Name); \
        return Module; \
    }

} /* namespace ZE */