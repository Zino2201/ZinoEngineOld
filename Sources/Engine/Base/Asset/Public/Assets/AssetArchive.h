#pragma once

#include "Serialization/BinaryArchive.h"
#include "AssetMetadata.h"
#include "Reflection/Serialization.h"

namespace ze
{

template<typename Archive>
struct AssetArchive
{
	const AssetMetadata& metadata;
	Archive underlying_archive;
	bool is_editor;
	bool is_loading;
	
	/** Cooked data provided by the cooker */
	std::vector<uint8_t> cooked_data;

	template<typename... Args>
	AssetArchive(const AssetMetadata& in_metadata, Args&&... in_args) : metadata(in_metadata),
		underlying_archive(std::forward<Args>(in_args)...)
	{
		is_editor = ZE_WITH_EDITOR;
		is_loading = serialization::is_input_archive<Archive>;
	}

	template<typename T>
	ZE_FORCEINLINE void operator<=>(const T& cdata)
	{
		T& data = const_cast<T&>(cdata);

		if constexpr (ze::serialization::has_serialize_function_with_version<T, AssetArchive<Archive>>)
		{
			uint32_t version = ze::serialization::TypeVersion<T>::version;
			underlying_archive <=> version;
			serialize(*this, data, version);
		}
		else if constexpr (ze::serialization::has_serialize_method_with_version<T, AssetArchive<Archive>>)
		{
			uint32_t version = ze::serialization::TypeVersion<T>::version;
			underlying_archive <=> version;
			data.serialize(*this, version);
		}
		else if constexpr (ze::serialization::has_serialize_function<T, AssetArchive<Archive>>)
		{
			serialize(*this, data);
		}
		else if constexpr (ze::serialization::has_serialize_method<T, AssetArchive<Archive>>)
		{
			data.serialize(*this);
		}
		else if constexpr (ze::serialization::is_serializable<T, Archive>)
		{
			underlying_archive <=> data;
		}
	}
};

template<typename Archive>
	requires ze::serialization::is_input_archive<Archive>
struct AssetInputArchive : public AssetArchive<Archive>
{
	template<typename... Args>
	AssetInputArchive(const AssetMetadata& in_metadata, Args&&... in_args) : AssetArchive<Archive>(in_metadata, std::forward<Args>(in_args)...) {}
};

template<typename Archive>
	requires ze::serialization::is_output_archive<Archive>
struct AssetOutputArchive : public AssetArchive<Archive>
{
	template<typename... Args>
	AssetOutputArchive(const AssetMetadata& in_metadata, Args&&... in_args) : AssetArchive<Archive>(in_metadata, std::forward<Args>(in_args)...) {}
};

/** std::vector support for AssetArchive */
template<typename Archive, typename T>
ZE_FORCEINLINE void serialize(AssetArchive<Archive>& archive, std::vector<T>& vector)
{
	typename std::vector<T>::size_type size = vector.size();

	archive <=> serialization::make_size(size);
	if constexpr (serialization::is_input_archive<Archive>)
		vector.resize(size);

	for (auto& elem : vector)
		archive <=> elem;
}

}

ZE_REFL_REGISTER_ARCHIVE(ze::AssetInputArchive<ze::serialization::BinaryInputArchive>);
ZE_REFL_REGISTER_ARCHIVE(ze::AssetOutputArchive<ze::serialization::BinaryOutputArchive>);