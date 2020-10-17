#pragma once

#include "Resource.h"

namespace ze
{

enum class ERSFilter
{
    Nearest,
    Linear,
};

enum class ERSSamplerAddressMode
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class ERSBorderColor
{
    FloatTransparentBlack,
    IntTransparentBlack,
    FloatOpaqueBlack,
    IntOpaqueBlack,
    FloatOpaqueWhite,
    IntOpaqueWhite,
};

/**
 * Create info for a sampler
 */
struct SRSSamplerCreateInfo
{
    ERSFilter MinFilter;
    ERSFilter MagFilter;
    ERSFilter MipMapMode;
    ERSSamplerAddressMode AddressModeU;
    ERSSamplerAddressMode AddressModeV;
    ERSSamplerAddressMode AddressModeW;
    float MipLodBias;
    ERSComparisonOp CompareOp;
    bool bAnistropyEnable;
    float MaxAnistropy;
    float MinLOD;
    float MaxLOD;

    SRSSamplerCreateInfo(
        const ERSFilter& InMinFilter = ERSFilter::Linear,
        const ERSFilter& InMagFilter = ERSFilter::Linear,
        const ERSFilter& InMipMapMode = ERSFilter::Linear,
        const ERSSamplerAddressMode& InAddressModeU = ERSSamplerAddressMode::Repeat,
        const ERSSamplerAddressMode& InAddressModeV = ERSSamplerAddressMode::Repeat,
        const ERSSamplerAddressMode& InAddressModeW = ERSSamplerAddressMode::Repeat,
        const float& InMipLodBias = 0.f,
        const ERSComparisonOp& InCompareOp = ERSComparisonOp::Never,
        const bool& bInAnistropyEnable = false,
        const float& InMaxAnistropy = 0.f,
        const float& InMinLOD = 0.f,
        const float& InMaxLOD = 0.f) : MinFilter(InMinFilter), MagFilter(InMagFilter),
            MipMapMode(InMipMapMode), AddressModeU(InAddressModeU), AddressModeV(InAddressModeV),
            AddressModeW(InAddressModeW), MipLodBias(InMipLodBias), CompareOp(InCompareOp),
            bAnistropyEnable(bInAnistropyEnable), MaxAnistropy(InMaxAnistropy), MinLOD(InMinLOD),
            MaxLOD(InMaxLOD){}
};

/**
 * A sampler
 */
class CRSSampler : public CRSResource
{
public:
    CRSSampler(const SRSSamplerCreateInfo& InCreateInfo) : CreateInfo(InCreateInfo) {}

    ZE_FORCEINLINE const SRSSamplerCreateInfo& GetCreateInfo() const { return CreateInfo; }
protected:
    SRSSamplerCreateInfo CreateInfo;
};

}