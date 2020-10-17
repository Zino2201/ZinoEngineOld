#pragma once

#include "Resource.h"

namespace ze
{

/** Fwds */
class CRSTexture;

struct SRSSurfaceCreateInfo
{
    void* WindowHandle;
    uint32_t Width;
    uint32_t Height;
    bool bEnableVSync;

	SRSSurfaceCreateInfo(void* InWindowHandle, const uint32_t& InWidth, 
        const uint32_t& InHeight,
		const bool& bInEnableVSync) : WindowHandle(InWindowHandle),
		Width(InWidth), Height(InHeight), bEnableVSync(bInEnableVSync) {}
};

/**
 * An surface
 * Contains its own swap chain
 */
class CRSSurface : public CRSResource
{
public:
    CRSSurface(const SRSSurfaceCreateInfo& InCreateInfo) : CreateInfo(InCreateInfo) {}

    virtual EFormat GetSwapChainFormat() const = 0;
    virtual void Resize(const uint32_t& InWidth, const uint32_t& InHeight) = 0;
    virtual CRSTexture* GetBackbufferTexture() = 0;
    
    ZE_FORCEINLINE const SRSSurfaceCreateInfo& GetCreateInfo() const { return CreateInfo; }
protected:
    SRSSurfaceCreateInfo CreateInfo;
};

}