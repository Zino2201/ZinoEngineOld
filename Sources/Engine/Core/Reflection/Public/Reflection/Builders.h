#pragma once

#include "Class.h"

namespace ZE::Refl::Builders
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
template<typename T, typename U = CType>
class TTypeBuilder
{
public:
	TTypeBuilder(const char* InName)
    {
        must(!CType::Get(TTypeName<T>::Name)); // Duplicated type

        Type = CType::RegisterType<U>(InName, sizeof(T));
    }

    virtual ~TTypeBuilder() = default;
protected:
    TNonOwningPtr<CType> Type;
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
        Struct = static_cast<CStruct*>(Type);

        CStruct::AddStruct(Struct);

        for(const auto& ParentClass : T::GetParentClasses())
        {
            auto PotentialParent = CStruct::Get(ParentClass);

            must(PotentialParent);
            if(PotentialParent)
                Struct->AddParent(PotentialParent);
        }
    }

	template<typename... Args>
	TStructBuilder<T, U>& Ctor()
	{
        std::function<void*(Args...)> Functor = &T::Refl_InternalInstantiate<Args...>;
		Struct->AddInstantiateFunc<Args...>(Functor);
		return *this;
	}

    template<typename P>
    TStructBuilder<T, U>& Property(const char* InName, P&& InPtr)
    {
        size_t Offset = (char*)&((T*)nullptr->*InPtr) - (char*)nullptr;
        Struct->AddProperty(CProperty(InName, sizeof(P), Offset));
        return *this;
    }
protected:
    TNonOwningPtr<CStruct> Struct;
};

template<typename T, typename U = CClass>
class TClassBuilder final : public TStructBuilder<T, U>
{
public:
	TClassBuilder(const char* InName) : TStructBuilder<T, U>(InName), Class(nullptr)
	{  
        Class = static_cast<CClass*>(Struct);

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
private:
    TNonOwningPtr<CClass> Class;
};

}