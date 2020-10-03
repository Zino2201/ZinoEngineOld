#pragma once

#include "EngineCore.h"
#include "Reflection/Macros.h"
#include <optional>
#include <string>
#include "NonCopyable.h"
#include "Type.gen.h"

namespace ZE::Refl
{

/**
 * Represents a simple C++ type
 */
ZCLASS()
class REFLECTION_API CType : public CNonCopyable
{
    ZE_REFL_BODY()

public:
    CType(const char* InName,
		const uint64_t& InSize)
        : Name(InName), Size(InSize) {}

    virtual ~CType() = default;

    const char* GetName() const { return Name; }
    uint64_t GetSize() const { return Size; }
protected:
    /** Pretty name */
    const char* Name;

    /** Type size */
    uint64_t Size;
};

/**
 * Register a new CType (will take ownership !)
 */
REFLECTION_API const CType* RegisterType(TOwnerPtr<CType> InType);

REFLECTION_API const CType* GetTypeByName(const std::string& InName);

template<typename T>
ZE_FORCEINLINE const CType* GetType()
{
    return GetTypeByName(TTypeName<T>::Name);
}

/**
 * Lazy pointer to a CType*
 * This can be used in reflection registration code since registration order is undefined
 */
template<typename T>
class TLazyTypePtr
{
public:
    TLazyTypePtr() : Name(""), Type(nullptr) {}

    explicit TLazyTypePtr(const std::string& InName) 
        : Name(InName), Type(nullptr) { TryGetType(); }

    ZE_FORCEINLINE const T* Get() const
    {
        return TryGetType();
    }
private:
    const T* TryGetType() const
    {
        if(!Type)
            Type = static_cast<const T*>(GetTypeByName(Name));
        return Type;
    }
private:
    std::string Name;
    mutable const T* Type;
};

}