#pragma once

#include "EngineVer.h"
#include "Serialization/Types/String.h"

namespace ze
{

/**
 * Header of a asset
 */
struct AssetHeader
{
    char id[7];
    ZEVersion engine_ver;
    std::string class_name;

    AssetHeader() : id("") {}
    AssetHeader(const std::string& in_class_name) : id("ZASSET"),
        engine_ver(get_version()),
        class_name(in_class_name) {}

	template<typename Archive>
	ZE_FORCEINLINE void serialize(Archive& archive)
	{
		archive <=> id;
		archive <=> engine_ver;
		archive <=> class_name;
	}

    bool is_valid() const
    {
        return strcmp(id, "ZASSET") == 0;
    }
};

ZE_FORCEINLINE AssetHeader make_asset_header(const std::string& in_class_name)
{
    return AssetHeader(in_class_name);
}

}