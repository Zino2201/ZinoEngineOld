#pragma once

#include "EngineCore.h"
#include "Reflection/Macros.h"
#include <optional>
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
    REFL_BODY()

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
        bool bDoesTypeExist = Get(InName);
        must(!bDoesTypeExist); // Duplicated type
        if(bDoesTypeExist)
            LOG(ELogSeverity::Fatal, None, "Error! Type %s (%d) is already registered",
                InName, InSize);

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


    const char* GetName() const { return Name; }
    static const std::vector<std::unique_ptr<CType>>& GetTypes() { return Types; }
    uint64_t GetSize() const { return Size; }
protected:
    /** Pretty name */
    const char* Name;

    /** Type size */
    uint64_t Size;
public:
    inline static std::vector<std::unique_ptr<CType>> Types;
};

}