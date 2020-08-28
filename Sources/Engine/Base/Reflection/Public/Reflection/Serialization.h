#pragma once

#include "Macros.h"

namespace ZE::Refl
{

/**
 * Serialization functions using Reflection system
 * This enable proper polymorphic serialization
 */
template<typename Archive, typename T>
	requires TIsArchive<Archive> && 
		TIsReflStruct<T>::Value || TIsReflClass<T>::Value
FORCEINLINE void Serialize(Archive& InArchive, T* InPtr)
{
	static_assert(false, "Use smart pointers instead of raw pointers for serializing reflected types");
}

/**
 * unique_ptr
 */
template<typename Archive, typename T>
	requires TIsArchive<Archive> && 
		TIsReflStruct<T>::Value || TIsReflClass<T>::Value
FORCEINLINE void Serialize(Archive& InArchive, std::unique_ptr<T>& InPtr)
{
	
}

}