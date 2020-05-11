#pragma once

#include "Struct.h"

namespace ZE
{
    
/**
 * Safe cast (only works with DECLARE_REFL_STRUCT_OR_CLASS types)
 */
template<typename To, typename From>
inline To* Cast(From* In)
{
    Refl::CStruct* FromStruct = In->GetStruct();
	Refl::CStruct* ToStruct = Refl::CStruct::Get<To>();

    if(FromStruct->IsDerivedFrom(ToStruct))
		return static_cast<To*>(In);

    return nullptr;
}

}