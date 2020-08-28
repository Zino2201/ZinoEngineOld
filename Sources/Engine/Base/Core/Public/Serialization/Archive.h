#pragma once

#include "EngineCore.h"

namespace ZE::Serialization
{
	
/**
 * Base interface for archives that serialize data
 */
class CORE_API IArchive
{
public:
	IArchive() : bIsSaving(false), bIsBinary(true) {}
	IArchive(const bool& bInBinary) : bIsSaving(false), bIsBinary(bInBinary) {}

	FORCEINLINE bool IsSaving() const { return bIsSaving; }
	FORCEINLINE bool IsBinary() const { return bIsBinary; }
protected:
	bool bIsSaving;
	bool bIsBinary;
};

/** Wrapper for a container size, used to omit size for special archives (e.g JSON) */
template<typename T>
struct TContainerSize
{
public:
	TContainerSize(const T& InSize) : Size(InSize) {}
	TContainerSize(T&& InSize) : Size(std::move(InSize)) {}
private:
	T Size;
};

/** Type traits */

class CInputArchive : public IArchive {};
class COutputArchive : public IArchive {};
class CORE_API CTextArchive {};

template<typename T>
static constexpr bool TIsTextArchive = std::is_base_of_v<CTextArchive, T>;

template<typename T>
static constexpr bool TIsOutputArchive = std::is_convertible_v<T, COutputArchive>;

template<typename T>
static constexpr bool TIsArchive = std::is_base_of_v<IArchive, T>;

/**
 * Global basic serialize function
 */
template<typename Archive, typename T>
FORCEINLINE void Serialize(Archive& InArchive, T& InData)
{
	InData.Serialize(InArchive);
}

/** Ops */
template<typename Archive, typename T>
	requires TIsArchive<Archive> && TIsOutputArchive<Archive>
FORCEINLINE Archive& operator<<(Archive& InArchive, T& InData)
{
	Serialize(InArchive, InData);
	return InArchive;
}

template<typename Archive, typename T>
	requires TIsArchive<Archive> && (!TIsOutputArchive<Archive>)
FORCEINLINE Archive& operator>>(Archive& InArchive, T& InData)
{
	Serialize(InArchive, InData);
	return InArchive;
}

template<typename Archive, typename T>
	requires TIsArchive<Archive>
FORCEINLINE Archive& operator<=>(Archive& InArchive, T& InData)
{
	Serialize(InArchive, InData);
	return InArchive;
}

#include "Funcs/Primitives.inl"
//#include "Funcs/Char.inl"
//#include "Funcs/Array.inl"
//#include "Funcs/Vector.inl"
//#include "Funcs/String.inl"

}