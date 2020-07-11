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

/**
 * Operators for std::array
 */
template<typename T, size_t Count>
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const std::array<T, Count>& InValue)
{
	if (!InArchive.IsSaving())
		return InArchive;

	for(const auto& Elem : InValue)
		InArchive << Elem;

	return InArchive;
}

template<typename T, size_t Count>
FORCEINLINE IArchive& operator>>(IArchive& InArchive, std::array<T, Count>& InValue)
{
	if (InArchive.IsSaving())
		return InArchive;

	for (auto& Elem : InValue)
		InArchive >> Elem;

	return InArchive;
}

/**
 * Operators for std::vector
 */
template<typename T>
FORCEINLINE IArchive& operator<<(IArchive& InArchive, const std::vector<T>& InValue)
{
	if (!InArchive.IsSaving())
		return InArchive;

	InArchive << InValue.size();

	for(const auto& Elem : InValue)
		InArchive << Elem;

	return InArchive;
}

template<typename T>
FORCEINLINE IArchive& operator>>(IArchive& InArchive, std::vector<T>& InValue)
{
	if (InArchive.IsSaving())
		return InArchive;
	
	/** Resize vector */
	size_t Size = 0;
	InArchive >> Size;
	InValue.resize(Size);
	
	/** Copy to vector */
	for(auto& Elem : InValue)
		InArchive >> Elem;

	return InArchive;
}

#include "Operators/String.inl"

#pragma endregion

}