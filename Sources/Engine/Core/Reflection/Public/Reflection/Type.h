#pragma once

#include "EngineCore.h"
#include "Reflection/Macros.h"
#include <optional>
#include "NonCopyable.h"

namespace ZE::Refl
{

/**
 * Represents a simple C++ type
 */
class REFLECTION_API CType : public CNonCopyable
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
        must(!Get(InName)); // Duplicated type

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
    
    static CType* Get(const std::string_view& InName);

    static const std::vector<std::unique_ptr<CType>>& GetTypes() { return Types; }

    const char* GetName() const { return Name; }
    uint64_t GetSize() const { return Size; }
protected:
    /** Pretty name */
    const char* Name;

    uint64_t Size;
public:
    inline static std::vector<std::unique_ptr<CType>> Types;
};
DECLARE_REFL_TYPE(CType);

}