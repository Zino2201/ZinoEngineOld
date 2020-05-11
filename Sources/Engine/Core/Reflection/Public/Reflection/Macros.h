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
void Refl_InitReflectedClassesAndStructs();

class CType;
class CStruct;
class CClass;

/** For macros only */
namespace Internal
{
	REFLECTION_API TNonOwningPtr<CStruct> GetStructByName(const char* InName);
    REFLECTION_API TNonOwningPtr<CClass> GetClassByName(const char* InName);
}
}

/**
 * Declare a reflected type
 */
#define DECLARE_REFL_TYPE(Type) \
    template<> struct TTypeName<Type> \
    { \
        inline static const char* Name = #Type; \
    }

/**
 * Declare a class with reflection data
 * This macro just add some required functions for the reflection/RTTI system
 */
#define DECLARE_REFL_STRUCT_OR_CLASS_INTERNAL(Class) \
    public: \
    friend void ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
    friend ZE::Refl::CType; \
    template<typename... Args> \
    static void* Refl_InternalInstantiate(Args&&... InArgs) { return new Class(std::forward<Args>(InArgs)...); } \
    template<typename... Args> \
    static void Refl_InternalPlacementNew(void* InPtr, Args&&... InArgs) { new (InPtr) Class(std::forward<Args>(InArgs)...); } \
    static TNonOwningPtr<ZE::Refl::CStruct> GetStaticStruct() { return ZE::Refl::Internal::GetStructByName(#Class); } \
    static TNonOwningPtr<ZE::Refl::CClass> GetStaticClass() { return ZE::Refl::Internal::GetClassByName(#Class); } \

#define DECLARE_REFL_STRUCT_OR_CLASS(Class) \
    DECLARE_REFL_STRUCT_OR_CLASS_INTERNAL(Class) \
    static const std::vector<const char*> GetParentClasses() { return {}; } \
    virtual TNonOwningPtr<ZE::Refl::CStruct> GetStruct() { return ZE::Refl::Internal::GetStructByName(#Class); }


#define DECLARE_REFL_STRUCT_OR_CLASS1(Class, A) \
    DECLARE_REFL_STRUCT_OR_CLASS_INTERNAL(Class) \
    static const std::vector<const char*> GetParentClasses() \
    { \
        return { #A }; \
    } \
    TNonOwningPtr<ZE::Refl::CStruct> GetStruct() override { return ZE::Refl::Internal::GetStructByName(#Class); }
