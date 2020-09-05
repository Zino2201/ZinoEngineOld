#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include <array>
#include "EngineVer.h"
#include "Serialization/Types/String.h"
#include "Asset.gen.h"

namespace ZE
{

namespace Serialization { class IArchive; }

/**
 * Header of a asset
 */
struct SAssetHeader
{
    const char Id[7];
    SZEVersion EngineVer;
    std::string ClassName;

    SAssetHeader(const std::string& InClassName) : Id("ZASSET"),
        EngineVer(GetZEVersion()),
        ClassName(InClassName) {}

	template<typename Archive>
	ZE_FORCEINLINE void Serialize(Archive& InArchive)
	{
		InArchive <=> Id;
		InArchive <=> EngineVer;
		InArchive <=> ClassName;
	}
};

ZE_FORCEINLINE SAssetHeader MakeAssetHeader(const std::string& InClassName)
{
    return SAssetHeader(InClassName);
}

/**
 * Base class for assets
 * This class provide safe serialization by checking versions
 * To implement a custom serialization system, override Serialize
 */
ZCLASS()
class ENGINE_API CAsset
{
    REFL_BODY()

public:
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