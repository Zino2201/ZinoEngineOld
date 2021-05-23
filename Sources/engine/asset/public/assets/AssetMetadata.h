#pragma once

#include "serialization/types/Uuid.h"
#include "serialization/Json.h"
#include "EngineVer.h"
#include "reflection/Class.h"

namespace ze
{

/**
 * Describes the content of a asset metadata file
 * This file contains required metadata about a asset and tell the engine how to open it
 * 
 * The engine workflow for loading an asset is:
 *  - Load & deserialize the corresponding metadata file (must be a asset_name.asset_extension.zemeta file)
 *  - Use the unserialized infos to load and understand the correspond asset file (.zetexture, .zemodel, ...)
 * 
 * This allows the engine to have text or binary assets like Unity unlike Unreal where everything is a binary .uasset
 * The metadata file can also store additional data like import settings
 */
struct AssetMetadata
{
    enum Version
    {
        Ver0
    };

    /** Asset UUID */
    uuids::uuid uuid;

    /** Engine version this asset use */
    ZEVersion engine_version;

    /** Asset class */
    const reflection::Class* asset_class;

    /** 
     * The true format of the asset
     * This is value set by the serializer/cooker and is asset-specific
     * For example, Texture asset may have a Editor format (that store only the source data and use the asset datacache) and a Cooked format (processed by the cooker; that store all mips)
     */
    uint32_t asset_format;

    /** Does this asset have seperate cooked data (e.g for streaming) */
    bool has_seperate_cooked_data;

    robin_hood::unordered_map<std::string, std::string> editor_data;

    AssetMetadata() : asset_class(nullptr), asset_format(0), has_seperate_cooked_data(false) {}

    template<typename ArchiveType>
        requires std::is_base_of_v<serialization::JsonArchive, ArchiveType>
    void serialize(ArchiveType& archive)
    {
        archive <=> serialization::make_named_data("uuid", uuid);
        if constexpr(serialization::is_input_archive<ArchiveType>)
        {
            std::string class_name;
            archive <=> serialization::make_named_data("class", class_name);
            asset_class = reflection::Class::get_by_name(class_name);
        }
        else
        {
            archive <=> serialization::make_named_data("class", asset_class->get_name());
        }
        archive <=> serialization::make_named_data("engine_version", engine_version);
        archive <=> serialization::make_named_data("asset_format", asset_format);
        archive <=> serialization::make_named_data("has_seperate_cooked_data", has_seperate_cooked_data);
#if ZE_WITH_EDITOR
        if(editor_data.size() > 0)
        	archive <=> serialization::make_named_data("editor_data", editor_data);
#endif
    }
};
ZE_SERL_TYPE_VERSION(AssetMetadata, AssetMetadata::Ver0);

}