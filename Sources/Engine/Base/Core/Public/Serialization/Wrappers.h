#pragma once

namespace ZE::Serialization
{

/** Wrapper for a size, used to omit size for special archives (e.g JSON) */
template<typename T>
struct TSize
{
	T Size;
	
	TSize(T&& InSize) : Size(std::forward<T>(InSize)) {}
};

template<typename T>
ZE_FORCEINLINE TSize<T> MakeSize(T&& InSize)
{
	return TSize<T>(std::forward<T>(InSize));
}

/** Wrapper for contiguous binary data */
template<typename T>
struct TBinaryData
{
	void* Data;
	uint64_t Size;

	TBinaryData(T* InData, const uint64_t& InSize) : Data(InData), Size(InSize) {}

	operator bool() const { return !!Data;  }
};

template<typename T>
ZE_FORCEINLINE TBinaryData<T> MakeBinaryData(T* InData, const uint64_t& InSize)
{
	return TBinaryData<T>(InData, InSize);
}

/**
 * Data with a name
 */
template<typename T>
struct TNamedData
{
	std::string_view Name;
	T& Data;
	
	TNamedData(const std::string_view& InName, T& InData) : InName(Name), Data(InData) {}
};

}