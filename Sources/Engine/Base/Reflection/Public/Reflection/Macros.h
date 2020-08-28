/** ZRT:SKIP */

#pragma once

#include <optional>
#include <functional>

/**
 * Macros to use for the reflection system
 */

/**
 * Allow to gather a type name in runtime
 * Should be specialized using DECLARE_REFL_TYPE
 */
template<typename T>
struct TTypeName;

/** Forward declarations */
namespace ZE::Refl
{

/** Forward decls */
static void Refl_InitReflectedClassesAndStructs();

class CType;
class CStruct;
class CClass;


/** Type traits */

/**
 * Is the type a CStruct
 */
template<typename T>
struct TIsReflStruct
{
    static constexpr bool Value = false;
};

/**
 * Is the type a CClass
 */
template<typename T>
struct TIsReflClass
{
	static constexpr bool Value = false;
};


/** For macros only */
namespace Internal
{
	REFLECTION_API CStruct* GetStructByName(const char* InName);
    REFLECTION_API CClass* GetClassByName(const char* InName);
}
}

/**
 * Macros parsed by the reflection tool
 */
#define ZSTRUCT(...)
#define ZCLASS(...)
#define ZPROPERTY(...)
#define ZFUNCTION(...)

/** Macro used for ZRT to generate a body */
#define REFL_BODY() CONCAT(CONCAT(CONCAT(_Refl_Body_, CURRENT_FILE_UNIQUE_ID), _), __LINE__)

/**
 * Specialize TTypeName
 */
#define REFL_SPECIALIZE_TYPE_NAME(Type, TypeName) \
    template<> struct TTypeName<Type> \
    { \
        inline static const char* Name = TypeName; \
    }

/**
 * ZRT ONLY MACROS
 */
#define DECLARE_STRUCT(Struct, StructName) \
    public: \
    friend ZE::Refl::CType; \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    template<typename... Args> \
    static void* Refl_InternalInstantiate(Args&&... InArgs) { return new Struct(std::forward<Args>(InArgs)...); } \
    template<typename... Args> \
    static void Refl_InternalPlacementNew(void* InPtr, Args&&... InArgs) { new (InPtr) Struct(std::forward<Args>(InArgs)...); } \
    static ZE::Refl::CStruct* GetStaticStruct(); \
    virtual ZE::Refl::CStruct* GetStruct() const { return ZE::Refl::Internal::GetStructByName(StructName); }

#define DECLARE_ABSTRACT_CLASS(Class, ClassName) \
    public: \
    friend ZE::Refl::CType; \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    static ZE::Refl::CClass* GetStaticClass(); \
    virtual ZE::Refl::CClass* GetClass() const { return ZE::Refl::Internal::GetClassByName(ClassName); } \
    private:

#define DECLARE_CLASS(Class, ClassName) \
    public: \
    friend ZE::Refl::CType; \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    template<typename... Args> \
        requires (!std::is_abstract_v<Class>) \
    static void* Refl_InternalInstantiate(Args&&... InArgs) { return new Class(std::forward<Args>(InArgs)...); } \
    template<typename... Args> \
        requires (!std::is_abstract_v<Class>) \
    static void Refl_InternalPlacementNew(void* InPtr, Args&&... InArgs) { new (InPtr) Class(std::forward<Args>(InArgs)...); } \
    static ZE::Refl::CClass* GetStaticClass(); \
    virtual ZE::Refl::CClass* GetClass() const { return ZE::Refl::Internal::GetClassByName(ClassName); } \
    private: