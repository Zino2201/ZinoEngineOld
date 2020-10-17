#pragma once

#include "Resource.h"

namespace ze
{

/**
 * Texture usage
 */
enum class ERSTextureUsageFlagBits
{
    None = 0,
  
    Sampled = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2
};
ENABLE_FLAG_ENUMS(ERSTextureUsageFlagBits, ERSTextureUsageFlags);

/**
 * Texture type
 */
enum class ERSTextureType
{
    Tex1D,
    Tex2D,
    Tex3D
};

struct SRSTextureCreateInfo
{
    ERSTextureType Type;
    ERSTextureUsageFlags Usage;
    ERSMemoryUsage MemoryUsage;
    EFormat Format;
    uint32_t Width;
    uint32_t Height;
    uint32_t Depth;
    uint32_t ArraySize;
    uint32_t MipLevels;
    ESampleCount SampleCount;

	SRSTextureCreateInfo(
		const ERSTextureType& InTextureType,
		const ERSTextureUsageFlags& InTextureUsage,
		const ERSMemoryUsage& InMemoryUsage,
		const EFormat& InFormat,
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth,
		const uint32_t& InArraySize,
		const uint32_t& InMipLevels,
		const ESampleCount& InSampleCount) :
		Type(InTextureType),
		Usage(InTextureUsage), MemoryUsage(InMemoryUsage),
		Format(InFormat), Width(InWidth), Height(InHeight), Depth(InDepth), ArraySize(InArraySize),
		MipLevels(InMipLevels) {}
};

/**
 * A texture
 * Type is determined by ERSTextureType
 */
class CRSTexture : public CRSResource 
{
public:
	CRSTexture(const SRSTextureCreateInfo& InCreateInfo) : 
        CreateInfo(InCreateInfo) {}

    virtual void Copy(const void* Src) = 0;

    ZE_FORCEINLINE const SRSTextureCreateInfo& GetCreateInfo() const { return CreateInfo; }
protected:
    SRSTextureCreateInfo CreateInfo;
};

}