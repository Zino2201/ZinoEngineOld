#pragma once

#include "EngineCore.h"

namespace ZE::Serialization
{

template<typename ArchiveType>
class CORE_API TInputArchive
{
public:
	TInputArchive(ArchiveType& InArchive) : This(InArchive) {}
	
	template<typename T>
	ZE_FORCEINLINE ArchiveType& operator<=>(T&& InData)
	{
		Serialize(This, InData);
		return This;
	}
private:
	ArchiveType& This;
};

template<typename ArchiveType>
class CORE_API TOutputArchive
{
public:
	TOutputArchive(ArchiveType& InArchive) : This(InArchive) {}

	template<typename T>
	ZE_FORCEINLINE ArchiveType& operator<=>(const T& InData)
	{
		Serialize(This, const_cast<T&>(InData));
		return This;
	}
private:
	ArchiveType& This;
};

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

/** Type traits for archives */
namespace Traits 
{
	struct TextArchive {};
}

template<typename T>
static constexpr bool TIsInputArchive = std::is_base_of_v<TInputArchive<T>, T>;

template<typename T>
static constexpr bool TIsOutputArchive = std::is_base_of_v<TOutputArchive<T>, T>;

template<typename T>
static constexpr bool TIsArchive = TIsInputArchive<T> || TIsOutputArchive<T>;

template<typename T>
static constexpr bool TIsTextArchive = std::is_base_of_v<Traits::TextArchive, T>;

template<typename C, typename Ret, typename... Args>
struct THasSerializeMethod
{
private:
	template<typename T>
	static constexpr auto Check(T*) -> typename std::is_same<
		decltype(std::declval<T>().Serialize(std::declval<Args>()...)), Ret>::type;

	template<typename>
	static constexpr std::false_type Check(...);

	typedef decltype(Check<C>(0)) Type;

public:
	static constexpr bool Value = Type::value;
};

/**
 * Basic serialize function
 */
template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>) && THasSerializeMethod<T, void, T&>::Value
ZE_FORCEINLINE void Serialize(Archive& InArchive, T& InData)
{
	InData.Serialize(InArchive);
}

}