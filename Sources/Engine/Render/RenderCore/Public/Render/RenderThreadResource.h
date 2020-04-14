#pragma once

namespace ZE
{

/**
 * An resource owned by render thread
 */
class RENDERCORE_API CRenderThreadResource
{
public:
    virtual ~CRenderThreadResource() { if(bIsInitialized) DestroyResource(); }

    virtual void InitResource();
    virtual void DestroyResource();
protected:
    virtual void InitResource_RenderThread() { bIsInitialized = true; }
    virtual void DestroyResource_RenderThread() { bIsInitialized = false; }
private:
    bool bIsInitialized = false;
};

}