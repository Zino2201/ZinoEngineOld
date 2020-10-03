#pragma once

#include "Class.h"
#include "Enum.h"

namespace ZE::Refl::Builders
{

/**
 * Helpers utilites to build types
 */
#define ZE_REFL_INIT_BUILDERS_FUNC(UniqueName) ZE_REFL_INIT_BUILDERS_FUNC_INTERNAL(UniqueName)
#define ZE_REFL_INIT_BUILDERS_FUNC_INTERNAL(c) \
    struct ZE_CONCAT(_Refl_AutoInit, c) \
    { \
        ZE_CONCAT(_Refl_AutoInit, c) () \
	    { \
            ZE::Refl::Refl_InitReflectedClassesAndStructs(); \
        } \
    }; \
    static ZE_CONCAT(_Refl_AutoInit, c) ZE_CONCAT(Refl_AutoInitStruct, c); \
    static void Refl_InitReflectedClassesAndStructs()

/**
 * Simple type builder
 */
template<typename T, typename U = CType>
class TTypeBuilder
{
public:
	TTypeBuilder(const char* InName)
    {
        Type = const_cast<CType*>(RegisterType(new U(InName, sizeof(T))));
    }

    virtual ~TTypeBuilder() = default;
protected:
    CType* Type;
};

/**
 * Enum builder
 */
template<typename T>
struct TEnumBuilder : public TTypeBuilder<T, CEnum>
{
public:
    using TUnderlyingType = typename std::underlying_type<T>::type;

    TEnumBuilder(const char* InName) : TTypeBuilder<T, CEnum>(InName)
    {
        Enum = static_cast<CEnum*>(TEnumBuilder::Type);

        RegisterEnum(Enum);
        Enum->SetUnderlyingType(TLazyTypePtr<CType>(TTypeName<TUnderlyingType>::Name));
    }

    template<typename E>
    TEnumBuilder& Value(const std::string& InName,
        const E& InEnum)
    {
        Enum->AddValue(InName, static_cast<TUnderlyingType>(InEnum));

        return *this;
    }

    CEnum* GetEnum() const { return Enum; }
private:
    CEnum* Enum;
};

/**
 * A struct builder
 */
template<typename T, typename U = CStruct>
class TStructBuilder : public TTypeBuilder<T, U>
{
public:
    TStructBuilder(const char* InName) : TTypeBuilder<T, U>(InName)
    {
        Struct = static_cast<CStruct*>(TStructBuilder::Type);

        /** Only register as a struct if it is a struct* */
        if constexpr(std::is_same_v<U, CStruct>)
            RegisterStruct(Struct);
    }

	template<typename... Args>
	TStructBuilder<T, U>& Ctor()
	{
        if constexpr(!std::is_abstract_v<T>)
        {
			std::function<void* (Args...)> Functor = &T::template ZE__Refl_InternalInstantiate<Args...>;
			Struct->AddInstantiateFunc<Args...>(Functor);

			std::function<void(void*, Args...)> Functor2 = &T::template ZE__Refl_InternalPlacementNew<Args...>;
			Struct->AddPlacementNewFunc<Args...>(Functor2);
        }

		return *this;
	}

    TStructBuilder<T, U>& Parent(const char* InParent)
    {
		Struct->AddParent(InParent);

        return *this;
    }

    template<typename P>
    TStructBuilder<T, U>& Property(const char* InName, P&& InPtr,
        const EPropertyFlags& InFlags)
    {
        size_t Offset = (char*)&((T*)nullptr->*InPtr) - (char*)nullptr;
        Struct->AddProperty(CProperty(InName, sizeof(P), Offset, InFlags));
        return *this;
    }

    CStruct* GetStruct() const { return Struct; }
protected:
    CStruct* Struct;
};

template<typename T, typename U = CClass>
class TClassBuilder final : public TStructBuilder<T, U>
{
public:
	TClassBuilder(const char* InName) : TStructBuilder<T, U>(InName), Class(nullptr)
	{  
        Class = static_cast<CClass*>(TClassBuilder::Struct);

		RegisterClass(Class);
	}

    /**
     * Mark the class as a interface
     */
    TClassBuilder<T, U>& MarkAsInterface()
    {
        Class->SetIsInterface(true);
        return *this;
    }

    CClass* GetClass() const { return Class; }
private:
    CClass* Class;
};

}