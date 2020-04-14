#pragma once

#include "EngineCore.h"
#include <unordered_map>

/**
 * ZinoEngine custom reflection and RTTI library
 * Inspired from RTTR and UE4
 */
namespace ZE::Refl
{

/**
 * A property
 */
class REFLECTION_API CProperty
{
public:
	CProperty(const char* InName,
		const uint64_t& InSize,
		const size_t& InOffset) :
		Name(InName), Size(InSize), Offset(InOffset) {}

	void* GetData(void* InPtr) const
	{
		uint8_t* Dst = reinterpret_cast<uint8_t*>(InPtr);
		return Dst + Offset;
	}

	template<typename T>
	T* GetData(void* InPtr) const
	{
		return reinterpret_cast<T*>(GetData(InPtr));
	}
private:
    const char* Name;
    size_t Size;
	size_t Offset;
};

/**
 * A function
 */
class REFLECTION_API CFunction
{
};

/**
 * Represents a C++ struct
 */
struct STypeStruct
{
    std::vector<CProperty> Properties;
};

/**
 * Represents a C++ class
 */
struct STypeClass : public STypeStruct
{
    std::vector<CFunction> Functions;
};

template<typename T>
struct TTypeName;

/**
 * Represents a C++ type
 * Also contains helper functions
 */
class CType
{
public:
    CType(const char* InName,
		const uint64_t& InSize)
        : Name(InName), Size(InSize) {}

    CType(const char* InName,
        const uint64_t& InSize,
        const STypeStruct& InStruct) : Name(InName), Size(InSize), Struct(InStruct) {} 
    ~CType() {}

    /**
     * Register a simple type
     */
    REFLECTION_API static void RegisterType(const char* InName,
        const uint64_t& InSize);

    /**
     * Register a struct
     */
    REFLECTION_API static void RegisterStruct(const char* InName,
        const uint64_t& InSize,
        const STypeStruct& InStruct);

    template<typename T>
    static CType* Get()
    {
        /**
         * If you have an error
         * Then you haven't used DECLARE_REFL_STRUCT_OR_CLASS on your class/struct 
         */
        const char* TypeName = T::ZEReflClassName;

        for(const auto& Type : Types)
        {
            if(Type->Name == TypeName)
                return Type.get();
        }

        return nullptr;
    }
protected:
    const char* Name;
    uint64_t Size;
    union
    {
        STypeStruct Struct;
        STypeClass Class;
    };
public:
    REFLECTION_API static std::vector<std::unique_ptr<CType>> Types;
};

/** Forward decls */
static void Refl_InitReflectedClassesAndStructs();

/**
 * Declare a class with reflection data
 * This macro just add some required functions for the reflection/RTTI system
 */
#define DECLARE_REFL_STRUCT_OR_CLASS() \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    friend CType; \
    public: \
    inline static const char* ZEReflClassName = ""; // Gived by builders

namespace Builders
{

/**
 * Helpers utilites to build types
 */
#define REFL_INIT_BUILDERS_FUNC() \
    struct Refl_AutoInit \
    { \
        Refl_AutoInit() \
	    { \
            ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
        } \
    }; \
    static Refl_AutoInit Refl_AutoInitStruct; \
    static void Refl_InitReflectedClassesAndStructs()

/**
 * Simple type builder
 */
template<typename T>
class TTypeBuilder
{
public:
	TTypeBuilder(const char* InName) : Name(InName) {}
	virtual ~TTypeBuilder() { Register(); }

    virtual void Register()
    {
        CType::RegisterType(Name, sizeof(T));
    }
protected:
    const char* Name;
};

/**
 * A struct builder
 */
template<typename T>
class TStructBuilder : public TTypeBuilder<T>
{
public:
    TStructBuilder(const char* InName) : TTypeBuilder<T>(InName) {}

    template<typename P>
    TStructBuilder<T>& Property(const char* InName, P&& InPtr)
    {
        size_t Offset = (char*)&((T*)nullptr->*InPtr) - (char*)nullptr;
        Properties.emplace_back(InName, sizeof(P), Offset);
        return *this;
    }

    /**
     * Complete class registration
     */
    virtual void Register() override
    {
        STypeStruct Struct;
        Struct.Properties = Properties;
        T::ZEReflClassName = Name;
        CType::RegisterStruct(Name, sizeof(T), Struct);
    }
protected:
    const char* Name;
    std::vector<CProperty> Properties;
};
}

}