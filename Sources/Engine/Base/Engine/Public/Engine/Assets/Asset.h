#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include <array>
#include "EngineVer.h"
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
};

/** Serialization operators */
template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, const SAssetHeader& InHeader)
{
	InArchive <=> InHeader.Id;
	InArchive <=> InHeader.EngineVer;
	InArchive <=> InHeader.ClassName;

	return InArchive;
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

    /**
     * Serialize the asset
     * Manages automatic serialization of this asset properties
     */
    template<typename Archive>
        requires Serialization::TIsArchive<Archive>
    void Serialize(Archive& InArchive)
    {
    }
};

template<typename T>
static constexpr bool TAssetUseCustomSerializer = false;

template<>
static constexpr bool TAssetUseCustomSerializer<int> = true;

}