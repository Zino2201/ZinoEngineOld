#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include <array>
#include <filesystem>
#include "Reflection/Serialization.h"
#include "Asset.gen.h"

namespace ZE
{

namespace Serialization { class IArchive; }

/**
 * Base class for assets
 * This class provide safe serialization by checking versions
 * To implement a custom serialization system, override Serialize
 */
ZCLASS()
class ASSET_API CAsset
{
    ZE_REFL_BODY()

public:
    virtual ~CAsset() = default;

    template<typename ArchiveType>
    void Serialize(ArchiveType& InArchive)
    {
        Refl::SerializeProperties(InArchive, *this);
    }

    void SetPath(const std::filesystem::path& InPath) { Path = InPath; }

    ZE_FORCEINLINE std::filesystem::path GetPath() const { return Path; }
protected:
    std::filesystem::path Path;
};


}