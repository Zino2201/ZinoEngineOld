#pragma once

#include "EngineCore.h"
#include <array>
#include <filesystem>
#include "AssetArchive.h"
#include "Reflection/Serialization.h"
#include "Reflection/BinaryArchiveRefl.h"
#include "Asset.gen.h"

namespace ze
{

/**
 * Base class for assets
 * This class provide safe serialization by checking versions
 */
ZCLASS()
class ASSET_API Asset
{
    ZE_REFL_BODY()

public:
    virtual ~Asset() = default;

    template<typename ArchiveType>
    void serialize(AssetArchive<ArchiveType>& in_archive) {}

    void set_path(const std::filesystem::path& in_path) { path = in_path; }
    void set_metadata(const AssetMetadata& in_metadata) { metadata = in_metadata; }

    const AssetMetadata& get_metadata() const { return metadata; }
    ZE_FORCEINLINE std::filesystem::path get_path() const { return path; }
    ZE_FORCEINLINE const uuids::uuid& get_uuid() const { return metadata.uuid; }
    ZE_FORCEINLINE const uint32_t& get_asset_format() const { return metadata.asset_format; }
protected:
    AssetMetadata metadata;
    std::filesystem::path path;
};


}