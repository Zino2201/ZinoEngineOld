#pragma once

#include "EngineCore.h"

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

/** Versionning */
template<typename T>
struct TTypeVersion
{
	constexpr static uint32_t Version = 0;
};

#define ZE_SERL_TYPE_VERSION(T, Ver) \
	template<> struct ZE::Serialization::TTypeVersion<T> { constexpr static uint32_t Version = Ver; };

/** Type traits for archives */
namespace Traits 
{
	struct TextArchive {};
}

template<typename T, typename Archive>
static constexpr bool THasSerializeFunction =
	requires(Archive& InArchive, T& InData) { Serialize(InArchive, InData); };

template<typename T, typename Archive>
static constexpr bool THasSerializeMethod =
	requires(Archive& InArchive, T& InData) { InData.Serialize(InArchive); };

template<typename T, typename Archive>
static constexpr bool THasSerializeFunctionWithVersion = 
	requires(Archive& InArchive, T& InData, const uint32_t& InVersion) { Serialize(InArchive, InData, InVersion); };

template<typename T, typename Archive>
static constexpr bool THasSerializeMethodWithVersion =
	requires(Archive& InArchive, T& InData, const uint32_t& InVersion) { InData.Serialize(InArchive, InVersion); };

template<typename ArchiveType>
class CORE_API TInputArchive
{
public:
	TInputArchive(ArchiveType& InArchive) : This(InArchive) {}

	template<typename T>
	ZE_FORCEINLINE ArchiveType& operator<=>(T&& InData)
	{
		if constexpr (THasSerializeFunctionWithVersion<T, Archive> ||
			THasSerializeMethodWithVersion<T, Archive>)
		{
			uint32_t Version = 0;
			This <=> Version;
			Serialize(This, InData, Version);
		}
		else
		{
			Serialize(This, InData);
		}

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
		if constexpr(THasSerializeFunctionWithVersion<T, ArchiveType> ||
			THasSerializeMethodWithVersion<T, ArchiveType>)
		{
			uint32_t Version = TTypeVersion<T>::Version;
			This <=> Version;
			Serialize(This, const_cast<T&>(InData), Version);
		}
		else
		{
			Serialize(This, const_cast<T&>(InData));
		}

		return This;
	}
private:
	ArchiveType& This;
};

template<typename T>
static constexpr bool TIsInputArchive = std::is_base_of_v<TInputArchive<T>, T>;

template<typename T>
static constexpr bool TIsOutputArchive = std::is_base_of_v<TOutputArchive<T>, T>;

template<typename T>
static constexpr bool TIsArchive = TIsInputArchive<T> || TIsOutputArchive<T>;

template<typename T>
static constexpr bool TIsTextArchive = std::is_base_of_v<Traits::TextArchive, T>;

/**
 * Basic serialize function
 */
template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>)
		&& THasSerializeMethod<T, Archive>
ZE_FORCEINLINE void Serialize(Archive& InArchive, T& InData)
{ 
	InData.Serialize(InArchive);
}

template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>) 
		&& THasSerializeMethodWithVersion<T, Archive> 
ZE_FORCEINLINE void Serialize(Archive& InArchive, T& InData, const uint32_t& InVersion)
{ 
	InData.Serialize(InArchive, InVersion);
}

}