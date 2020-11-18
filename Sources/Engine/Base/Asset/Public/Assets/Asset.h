#pragma once

#include "EngineCore.h"
#include <array>
#include <filesystem>
#include "Reflection/Serialization.h"
#include "Reflection/BinaryArchiveRefl.h"
#include "Asset.gen.h"

namespace ze
{

/**
 * Base class for assets
 * This class provide safe serialization by checking versions
 * To implement a custom serialization system, override Serialize
 */
ZCLASS()
class ASSET_API Asset
{
    ZE_REFL_BODY()

public:
    virtual ~Asset() = default;

    template<typename ArchiveType>
    void serialize(ArchiveType& in_archive)
    {
        //Refl::SerializeProperties(in_archive, *this);
    }

    void set_path(const std::filesystem::path& in_path) { path = in_path; }

    ZE_FORCEINLINE std::filesystem::path get_path() const { return path; }
protected:
    std::filesystem::path path;
};


}