#pragma once

#include "EngineCore.h"

namespace ZE::Serialization
{

/**
 * Base interface for archives
 */
class ENGINECORE_API IArchive
{
public:
	IArchive() : bIsSaving(false), bIsBinary(true) {}
	IArchive(const bool& bInBinary) : bIsSaving(false), bIsBinary(bInBinary) {}

	virtual void Serialize(const void* InData, const uint64_t& InSize) = 0;
	virtual void Deserialize(void* InData, const uint64_t& InSize) = 0;
	virtual void Flush() = 0;

	bool IsSaving() const { return bIsSaving; }
	bool IsBinary() const { return bIsBinary; }
protected:
	bool bIsSaving;
	bool bIsBinary;
};

#pragma region Operators
/**
 * Basic operator<<
 */
template<typename T>
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const T& InValue)
{
	static_assert(std::is_trivially_copyable_v<T>, "No operator<< found which can use T");
	if (!InArchive.IsSaving())
		return InArchive;
		
	InArchive.Serialize(&InValue, sizeof(T));

	return InArchive;
}

/**
 * Basic operator>>
 */
template<typename T>
FORCEINLINE IArchive& operator>>(IArchive& InArchive, T& InValue)
{
	static_assert(std::is_trivially_copyable_v<T>, "No operator>> found which can use T");
	if (InArchive.IsSaving())
		return InArchive;

	InArchive.Deserialize(&InValue, sizeof(T));

	return InArchive;
}

#include "Operators/Char.inl"
#include "Operators/Array.inl"
#include "Operators/Vector.inl"
#include "Operators/String.inl"

#pragma endregion

}