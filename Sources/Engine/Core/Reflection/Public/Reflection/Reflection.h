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
REFLECTION_API std::vector<CStruct*> GetDerivedStructsFrom(CStruct* InParent);
REFLECTION_API std::vector<CClass*> GetDerivedClassesFrom(CClass* InParent);

}

/** Basic types specializations */
REFL_SPECIALIZE_TYPE_NAME(bool, "bool");
REFL_SPECIALIZE_TYPE_NAME(long, "long");
REFL_SPECIALIZE_TYPE_NAME(float, "float");
REFL_SPECIALIZE_TYPE_NAME(double, "double");

REFL_SPECIALIZE_TYPE_NAME(uint8_t, "uint8_t");
REFL_SPECIALIZE_TYPE_NAME(uint16_t, "uint16_t");
REFL_SPECIALIZE_TYPE_NAME(uint32_t, "uint32_t");
REFL_SPECIALIZE_TYPE_NAME(uint64_t, "uint64_t");

REFL_SPECIALIZE_TYPE_NAME(int8_t, "int8_t");
REFL_SPECIALIZE_TYPE_NAME(int16_t, "int16_t");
REFL_SPECIALIZE_TYPE_NAME(int32_t, "int32_t");
REFL_SPECIALIZE_TYPE_NAME(int64_t, "int64_t");