#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include <array>
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
    REFL_BODY()

public:
    static constexpr std::string_view AssetFileExtension = "zasset";

    virtual ~CAsset() = default;

    template<typename Archive>
    void Serialize(Archive& InArchive)
    {
        
    }

    ZE_FORCEINLINE void SetPath(const std::string_view& InPath) { Path = InPath; }

    ZE_FORCEINLINE std::string GetPath() const { return Path;  }
protected:
    std::string Path;
};

}