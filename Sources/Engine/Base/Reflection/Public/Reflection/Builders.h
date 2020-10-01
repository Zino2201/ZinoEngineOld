#pragma once

#include "Class.h"

namespace ZE::Refl::Builders
{

/**
 * Helpers utilites to build types
 */
#define REFL_INIT_BUILDERS_FUNC(UniqueName) REFL_INIT_BUILDERS_FUNC_INTERNAL(UniqueName)
#define REFL_INIT_BUILDERS_FUNC_INTERNAL(c) \
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
        Type = CType::RegisterType<U>(InName, sizeof(T));
    }

    virtual ~TTypeBuilder() = default;
protected:
    CType* Type;
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

        CStruct::AddStruct(Struct);
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
		auto PotentialParent = CStruct::Get(InParent);

		if (PotentialParent)
        {
			Struct->AddParent(PotentialParent);
        }
		else
		{
			Struct->Refl_ParentsWaitingAdd.push_back(InParent);
		}

        return *this;
    }

    template<typename P>
    TStructBuilder<T, U>& Property(const char* InName, P&& InPtr)
    {
        size_t Offset = (char*)&((T*)nullptr->*InPtr) - (char*)nullptr;
        Struct->AddProperty(CProperty(InName, sizeof(P), Offset));
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

		CClass::AddClass(Class);
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