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
static void Refl_InitReflectedClassesAndStructs() ZE_USED;

class CType;
class CStruct;
class CClass;
class CEnum;

/** Type traits */

/**
 * Is the type a CStruct
 */
template<typename T>
    requires std::is_class_v<T>
static constexpr bool TIsReflStruct = false;

/**
 * Is the type a CClass
 */
template<typename T>
    requires std::is_class_v<T>
static constexpr bool TIsReflClass = false;

/**
 * Is the type a CEnum
 */
template<typename T>
    requires std::is_enum_v<T>
static constexpr bool TIsReflEnum = false;

template<typename T>
static constexpr bool TIsReflType = TIsReflStruct<T> ||
    TIsReflClass<T> || TIsReflEnum<T>;

template<typename T>
    requires std::is_enum_v<T>
ZE_FORCEINLINE const CEnum* GetStaticEnum() { return nullptr; }

template<typename T>
static constexpr bool TIsSerializableWithReflection = false;

}


/**
 * Macros parsed by the reflection tool
 */
#define ZSTRUCT(...)
#define ZCLASS(...)
#define ZENUM(...)
#define ZPROPERTY(...)
#define ZFUNCTION(...)

/** Macro used for ZRT to generate a body */
#define ZE_REFL_BODY() ZE_CONCAT(ZE_CONCAT(ZE_CONCAT(ZE_Refl_Body_, ZE_CURRENT_FILE_UNIQUE_ID), _), __LINE__)

/**
 * Specialize TTypeName
 */
#define ZE_REFL_SPECIALIZE_TYPE_NAME(Type, TypeName) \
    template<> struct TTypeName<Type> \
    { \
        inline static const char* Name = TypeName; \
    }

/**
 * Declare a reflected serializable type
 */
#define ZE_REFL_SERL_DECLARE_TYPE(Type) \
	namespace ZE::Refl \
	{ \
		template<> \
		static constexpr bool TIsSerializableWithReflection<Type> = true; \
	}

/**
 * ZRT ONLY MACROS
 */
#define ZE_REFL_DECLARE_STRUCT(Struct, StructName) \
    public: \
    friend ZE::Refl::CType; \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    template<typename... Args> \
    static void* ZE__Refl_InternalInstantiate(Args&&... InArgs) { return new Struct(std::forward<Args>(InArgs)...); } \
    template<typename... Args> \
    static void ZE__Refl_InternalPlacementNew(void* InPtr, Args&&... InArgs) { new (InPtr) Struct(std::forward<Args>(InArgs)...); } \
    static const ZE::Refl::CStruct* GetStaticStruct(); \
    virtual const ZE::Refl::CStruct* GetStruct() const;

#define ZE_REFL_DECLARE_ABSTRACT_CLASS(Class, ClassName) \
    public: \
    friend ZE::Refl::CType; \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    static const ZE::Refl::CClass* GetStaticClass(); \
    virtual const ZE::Refl::CClass* GetClass() const; \
    private:

#define ZE_REFL_DECLARE_CLASS(Class, ClassName) \
    public: \
    friend ZE::Refl::CType; \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    template<typename... Args> \
        requires (!std::is_abstract_v<Class>) \
    static void* ZE__Refl_InternalInstantiate(Args&&... InArgs) { return new Class(std::forward<Args>(InArgs)...); } \
    template<typename... Args> \
        requires (!std::is_abstract_v<Class>) \
    static void ZE__Refl_InternalPlacementNew(void* InPtr, Args&&... InArgs) { new (InPtr) Class(std::forward<Args>(InArgs)...); } \
    static const ZE::Refl::CClass* GetStaticClass(); \
    virtual const ZE::Refl::CClass* GetClass() const; \
    private:

#define ZE_REFL_DECLARE_ENUM(Enum, EnumWithoutNamespace) \
    const ZE::Refl::CEnum* ZE_CONCAT(ZE__Refl_GetStaticEnumImpl_, EnumWithoutNamespace)(); \
    namespace ZE::Refl \
    { \
        template<> ZE_FORCEINLINE const ZE::Refl::CEnum* GetStaticEnum<Enum>() { return ZE_CONCAT(ZE__Refl_GetStaticEnumImpl_, EnumWithoutNamespace)(); } \
    }