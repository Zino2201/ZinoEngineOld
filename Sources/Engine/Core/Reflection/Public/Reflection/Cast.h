#pragma once

#include "Struct.h"

namespace ZE
{
    
/**
 * Safe cast (only works with ZSTRUCT or ZCLASS types)
 */
template<typename To, typename From>
inline To* Cast(From* In)
{
    if(!In)
        return nullptr;

    Refl::CStruct* FromStruct = nullptr;

    if constexpr(Refl::TIsReflStruct<From>::Value)
        FromStruct = In->GetStruct();
    else
        FromStruct = static_cast<Refl::CStruct*>(In->GetClass());

    if(!FromStruct)
        return nullptr;

	Refl::CStruct* ToStruct = Refl::CStruct::Get<To>();
    if(!ToStruct)
        return nullptr;

    if(FromStruct->IsDerivedFrom(ToStruct))
		return static_cast<To*>(In);

    return nullptr;
}

}