#pragma once

#include "EngineCore.h"
#include "Reflection/Macros.h"
#include <optional>

namespace ZE::Refl
{

/**
 * Represents a simple C++ type
 */
class CType
{
    DECLARE_REFL_STRUCT_OR_CLASS(CType)

public:
    CType(const char* InName,
		const uint64_t& InSize)
        : Name(InName), Size(InSize) {}

    virtual ~CType() = default;

    /**
     * Register a type
     */
    template<typename T>
    static CType* RegisterType(const char* InName,
        const uint64_t& InSize)
    {
        T* Type = new T(InName, InSize);
		Types.push_back(std::unique_ptr<CType>(Type));
		return Type;
    }

    template<typename T>
    static CType* Get()
    {
		auto& Type = Get(TTypeName<T>::Name);
		must(Type); // This type is not registered if you hit this assert

		return Type;
    }
    
    REFLECTION_API static CType* Get(const std::string_view& InName);

    REFLECTION_API static const std::vector<std::unique_ptr<CType>>& GetTypes() { return Types; }

    const char* GetName() const { return Name; }
    uint64_t GetSize() const { return Size; }
protected:
    /** Pretty name */
    const char* Name;

    uint64_t Size;
public:
    REFLECTION_API inline static std::vector<std::unique_ptr<CType>> Types;
};
DECLARE_REFL_TYPE(CType);

}