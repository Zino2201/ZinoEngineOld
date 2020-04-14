#pragma once

#include <utility>
#include <string_view>
#include <vector>
#include <any>
#include "Object/Object.h"
/**
 * Basic elements for reflection system
 */

/**
 * Util object to build a struct
 */
template<typename T, typename U = CStruct>
class TStructBuilder
{
public:
    TStructBuilder(const char* InName) : Name(InName) {}

    /** Functions used to fill class */

    template<typename P>
    TStructBuilder<T, U>& Property(const char* InName, P&& InPtr)
    {
        size_t Offset = (char*)&((T*)nullptr->*InPtr) - (char*)nullptr;
        Properties.emplace_back(InName, Offset);
        return *this;
    }

    /**
     * Complete class registration
     */
    virtual TStructBuilder<T, U>& End()
    {
        CType::AddType<U>(Name, Properties);
        return *this;
    }
private:
    const char* Name;
    std::vector<CProperty> Properties;
};

/**
 * Util object to build a class
 */
template<typename T, typename U = CClass>
class TClassBuilder : public TStructBuilder<T, U>
{
public:
    TClassBuilder(const char* InName) : TStructBuilder(InName) {}
};
 
#define REFL_INIT_FUNC() static void Refl_InitReflectedClassesAndStructs(); \
    struct Refl_AutoInit \
    { \
        Refl_AutoInit() \
	    { \
            Refl_InitReflectedClassesAndStructs(); \
        } \
    }; \
    static Refl_AutoInit Refl_AutoInitStruct; \
    static void Refl_InitReflectedClassesAndStructs()