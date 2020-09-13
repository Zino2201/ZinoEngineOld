#pragma once

#include "EngineVer.h"
#include "Serialization/Types/String.h"

namespace ZE
{

/**
 * Header of a asset
 */
struct SAssetHeader
{
    char Id[7];
    SZEVersion EngineVer;
    std::string ClassName;

    SAssetHeader() : Id("") {}
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

    bool IsValid() const
    {
        return strcmp(Id, "ZASSET") == 0;
    }
};

ZE_FORCEINLINE SAssetHeader MakeAssetHeader(const std::string& InClassName)
{
    return SAssetHeader(InClassName);
}

}