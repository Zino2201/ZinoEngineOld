#pragma once

#include "EngineCore.h"
#include "Wrappers.h"

namespace ZE::Serialization
{

template<typename T>
struct THasVersion : std::false_type {};

/** Versionning */
template<typename T>
struct TTypeVersion
{
	static constexpr bool HasVersion = false;

	static_assert(HasVersion, "Please specify a version for T using ZE_SERL_TYPE_VERSION macro");
};

#define ZE_SERL_TYPE_VERSION(T, Ver) \
	template<> struct ZE::Serialization::TTypeVersion<T> { constexpr static uint32_t Version = Ver; constexpr static bool HasVersion = true; }; \

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

/**
 * Serialize function that call Serialize method on class/union if it exists
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

#include "Traits.h"