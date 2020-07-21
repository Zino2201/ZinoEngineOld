#pragma once

#include "Resource.h"

namespace ZE
{

constexpr uint32_t GMaxRenderTargetPerFramebuffer = 8;

/**
 * Details about a framebuffer (collection of render targets)
 * Used by BeginRenderPass
 */
struct SRSFramebuffer
{
    /** Color render targets */
    std::array<CRSTexture*, GMaxRenderTargetPerFramebuffer> ColorRTs;

    /** Depth render targets */
    std::array<CRSTexture*, GMaxRenderTargetPerFramebuffer> DepthRTs;

    SRSFramebuffer() 
    {
        memset(ColorRTs.data(), 0, sizeof(ColorRTs));
        memset(DepthRTs.data(), 0, sizeof(DepthRTs));
    }

	bool operator==(const SRSFramebuffer& InFramebuffer) const
	{
        return ColorRTs == InFramebuffer.ColorRTs && DepthRTs == InFramebuffer.DepthRTs;
	}
};

struct SRSFramebufferHash
{
	std::size_t operator()(const SRSFramebuffer& InFramebuffer) const noexcept
	{
		std::size_t Seed = 0;

		for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
		{
			HashCombine(Seed, InFramebuffer.ColorRTs[i]);
			HashCombine(Seed, InFramebuffer.DepthRTs[i]);
		}

		return Seed;
	}
};

}