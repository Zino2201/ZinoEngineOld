#pragma once

#include "Struct.h"

namespace ZE
{
    
/**
 * Safe cast (only works with ZSTRUCT or ZCLASS types)
 */
template<typename To, typename From>
    requires (Refl::TIsReflStruct<To> || Refl::TIsReflClass<To>) &&
        (Refl::TIsReflStruct<From> || Refl::TIsReflClass<From>)
inline To* Cast(From* In)
{
    if(!In)
        return nullptr;

    const Refl::CStruct* FromStruct = nullptr;

    if constexpr(Refl::TIsReflStruct<From>)
        FromStruct = In->GetStruct();
    else
        FromStruct = static_cast<const Refl::CStruct*>(In->GetClass());

    if(!FromStruct)
        return nullptr;

	const Refl::CStruct* ToStruct = Refl::GetStruct<To>();
    if(!ToStruct)
        return nullptr;

    if(FromStruct->IsDerivedFrom(ToStruct))
		return static_cast<To*>(In);

    return nullptr;
}

}