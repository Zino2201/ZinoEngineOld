#pragma once

#include "EngineCore.h"
#include "Wrappers.h"

namespace ze::serialization
{

template<typename T>
struct THasVersion : std::false_type {};

/** Versionning */
template<typename T>
struct TypeVersion
{
	static constexpr bool has_version = false;

	static_assert(has_version, "Please specify a version for T using ZE_SERL_TYPE_VERSION macro");
};

#define ZE_SERL_TYPE_VERSION(T, Ver) \
	template<> struct ze::serialization::TypeVersion<T> { constexpr static uint32_t version = Ver; constexpr static bool has_version = true; }; \

template<typename T, typename Archive>
static constexpr bool HasSerializeFunction =
	requires(Archive& archive, T& data) { serialize(archive, data); };

template<typename T, typename Archive>
static constexpr bool HasSerializeMethod =
	requires(Archive& archive, T& data) { data.serialize(archive); };

template<typename T, typename Archive>
static constexpr bool HasSerializeFunctionWithVersion = 
	requires(Archive& archive, T& data, const uint32_t& version) { serialize(archive, data, version); };

template<typename T, typename Archive>
static constexpr bool HasSerializeMethodWithVersion =
	requires(Archive& archive, T& data, const uint32_t& version) { data.serialize(archive, version); };

template<typename T, typename Archive>
static constexpr bool IsSerializable = HasSerializeFunction<T, Archive> ||
	HasSerializeMethod<T, Archive> ||
	HasSerializeFunctionWithVersion<T, Archive> ||
	HasSerializeMethodWithVersion<T, Archive>;

template<typename ArchiveType>
class CORE_API InputArchive
{
public:
	InputArchive(ArchiveType& archive) : this_(archive) {}

	template<typename T>
	ZE_FORCEINLINE ArchiveType& operator<=>(T&& data)
	{
		if constexpr (HasSerializeFunctionWithVersion<T, ArchiveType> ||
			HasSerializeMethodWithVersion<T, ArchiveType>)
		{
			uint32_t version = 0;
			this_ <=> version;
			serialize(this_, data, version);
		}
		else
		{
			serialize(this_, data);
		}

		return this_;
	}
private:
	ArchiveType& this_;
};

template<typename ArchiveType>
class CORE_API OutputArchive
{
public:
	OutputArchive(ArchiveType& archive) : this_(archive) {}

	template<typename T>
	ZE_FORCEINLINE ArchiveType& operator<=>(const T& data)
	{
		if constexpr(HasSerializeFunctionWithVersion<T, ArchiveType> ||
			HasSerializeMethodWithVersion<T, ArchiveType>)
		{
			uint32_t version = TypeVersion<T>::version;
			this_ <=> version;
			serialize(this_, const_cast<T&>(data), version);
		}
		else
		{
			serialize(this_, const_cast<T&>(data));
		}

		return this_;
	}
private:
	ArchiveType& this_;
};

/**
 * Serialize function that call Serialize method on class/union if it exists
 */
template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>)
		&& HasSerializeMethod<T, Archive>
ZE_FORCEINLINE void serialize(Archive& archive, T& data)
{ 
	data.serialize(archive);
}

template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>) 
		&& HasSerializeMethodWithVersion<T, Archive> 
ZE_FORCEINLINE void serialize(Archive& archive, T& data, const uint32_t& version)
{ 
	data.serialize(archive, version);
}

}

#include "Traits.h"