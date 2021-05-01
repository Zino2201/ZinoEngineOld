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
static constexpr bool has_serialize_function =
	requires(Archive& archive, T& data) { serialize(archive, data); };

template<typename T, typename Archive>
static constexpr bool has_serialize_function_const =
	requires(Archive& archive, const T& data) { serialize(archive, data); };

template<typename T, typename Archive>
static constexpr bool has_serialize_method =
	requires(Archive& archive, T& data) { data.serialize(archive); };

template<typename T, typename Archive>
static constexpr bool has_serialize_method_const =
	requires(Archive& archive, const T& data) { data.serialize(archive); };

template<typename T, typename Archive>
static constexpr bool has_serialize_function_with_version = 
	requires(Archive& archive, T& data, const uint32_t& version) { serialize(archive, data, version); };

template<typename T, typename Archive>
static constexpr bool has_serialize_function_with_version_const = 
	requires(Archive& archive, const T& data, const uint32_t& version) { serialize(archive, data, version); };

template<typename T, typename Archive>
static constexpr bool has_serialize_method_with_version =
	requires(Archive& archive, T& data, const uint32_t& version) { data.serialize(archive, version); };

template<typename T, typename Archive>
static constexpr bool has_serialize_method_with_version_const =
	requires(Archive& archive, const T& data, const uint32_t& version) { data.serialize(archive, version); };

template<typename T, typename Archive>
static constexpr bool has_any_const_serialize_function = has_serialize_function_const<T, Archive> ||
	has_serialize_method_const<T, Archive> ||
	has_serialize_function_with_version_const<T, Archive> ||
	has_serialize_method_with_version_const<T, Archive>;

template<typename T, typename Archive>
static constexpr bool is_serializable = has_serialize_function<T, Archive> ||
	has_serialize_function_const<T, Archive> ||
	has_serialize_method<T, Archive> ||
	has_serialize_method_const<T, Archive> ||
	has_serialize_function_with_version<T, Archive> ||
	has_serialize_function_with_version_const<T, Archive> ||
	has_serialize_method_with_version<T, Archive> ||
	has_serialize_method_with_version_const<T, Archive>;

template<typename T, typename Archive>
static constexpr bool do_support_loading = has_serialize_function<T, Archive> ||
	has_serialize_method<T, Archive> ||
	has_serialize_function_with_version<T, Archive> ||
	has_serialize_method_with_version<T, Archive>;

template<typename ArchiveType>
class CORE_API InputArchive
{
public:
	InputArchive(ArchiveType& archive) : this_(archive) {}

	template<typename T>
	ZE_FORCEINLINE ArchiveType& operator<=>(T&& data)
	{
		static_assert(do_support_loading<T, ArchiveType>, "T doesn't support loading! (no non-const-ref serialize function/method found)");

		pre_serialize(this_, const_cast<const T&>(data));

		if constexpr (has_serialize_function_with_version<T, ArchiveType> ||
			has_serialize_method_with_version<T, ArchiveType>)
		{
			uint32_t version = 0;
			this_ <=> make_named_data("version", version);
			serialize(this_, data, version);
		}
		else
		{
			serialize(this_, data);
		}

		post_serialize(this_, const_cast<const T&>(data));

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
		pre_serialize(this_, data);

		if constexpr(has_serialize_function_with_version<T, ArchiveType> ||
			has_serialize_function_with_version_const<T, ArchiveType> ||
			has_serialize_method_with_version<T, ArchiveType> ||
			has_serialize_method_with_version_const<T, ArchiveType>)
		{
			uint32_t version = TypeVersion<T>::version;
			this_ <=> make_named_data("version", version);
			serialize(this_, data, version);
		}
		else
		{
			serialize(this_, (std::conditional_t<has_any_const_serialize_function<T, ArchiveType>, const T&, T&>) data);
		}

		post_serialize(this_, data);
		
		return this_;
	}
private:
	ArchiveType& this_;
};

/** Pre-serialize and post-serialize, called before and after serialization. Useful for some serializers */
template<typename Archive, typename T>
void pre_serialize(Archive& archive, const T& data) {}

template<typename Archive, typename T>
void post_serialize(Archive& archive, const T& data) {}

/**
 * Serialize function that call Serialize method on class/union if it exists
 */
template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>)
		&& has_serialize_method<T, Archive>
ZE_FORCEINLINE void serialize(Archive& archive, T& data)
{ 
	data.serialize(archive);
}

template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>) 
		&& has_serialize_method_with_version<T, Archive> 
ZE_FORCEINLINE void serialize(Archive& archive, T& data, const uint32_t& version)
{ 
	data.serialize(archive, version);
}

/**
 * Const version
 */
template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>)
		&& has_serialize_method_const<T, Archive>
ZE_FORCEINLINE void serialize(Archive& archive, const T& data)
{ 
	data.serialize(archive);
}

template<typename Archive, typename T>
	requires (std::is_class_v<T> || std::is_union_v<T>) 
		&& has_serialize_method_with_version_const<T, Archive> 
ZE_FORCEINLINE void serialize(Archive& archive, const T& data, const uint32_t& version)
{ 
	data.serialize(archive, version);
}

}

#include "Traits.h"