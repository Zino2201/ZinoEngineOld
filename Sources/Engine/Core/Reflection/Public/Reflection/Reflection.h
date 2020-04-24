#pragma once

#include "EngineCore.h"
#include <unordered_map>
#include <algorithm>
#include "Reflection/Struct.h"
#include "Reflection/Class.h"
#include "Reflection/Cast.h"
#include "Reflection/Macros.h"

template<typename T>
struct TTypeName;

/**
 * ZinoEngine custom reflection and RTTI library
 * Inspired from RTTR and UE4
 */
namespace ZE::Refl
{

/**
 * Get all structs derived from struct
 */
REFLECTION_API std::vector<TNonOwningPtr<CStruct>> GetDerivedStructsFrom(const TNonOwningPtr<CStruct>& InParent);
REFLECTION_API std::vector<TNonOwningPtr<CClass>> GetDerivedClassesFrom(const TNonOwningPtr<CClass>& InParent);

}

/** Basic types */
DECLARE_REFL_TYPE(bool);
DECLARE_REFL_TYPE(int);
DECLARE_REFL_TYPE(long);
DECLARE_REFL_TYPE(float);
DECLARE_REFL_TYPE(double);

DECLARE_REFL_TYPE(uint8_t);
DECLARE_REFL_TYPE(uint16_t);
DECLARE_REFL_TYPE(uint32_t);
DECLARE_REFL_TYPE(uint64_t);

DECLARE_REFL_TYPE(int8_t);
DECLARE_REFL_TYPE(int16_t);
//DECLARE_REFL_TYPE(int32_t); (already defined as int)
DECLARE_REFL_TYPE(int64_t);