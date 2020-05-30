#pragma once

#include "EngineCore.h"
#include <any>
#include <algorithm>
#include "RenderThreadResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"

namespace ZE
{

/**
 * Wrapper around a CRSBuffer
 */
template<typename T, bool bUsePersistantMapping = true>
struct TUniformBuffer : public CRenderThreadResource
{
    void InitResource_RenderThread() override
    {
        ERSMemoryUsage MemoryUsage = ERSMemoryUsage::HostVisible;
        if(bUsePersistantMapping)
            MemoryUsage |= ERSMemoryUsage::UsePersistentMapping;

        Buffer = GRenderSystem->CreateBuffer(
            ERSBufferUsage::UniformBuffer,
            MemoryUsage,
            sizeof(T),
            SRSResourceCreateInfo(nullptr, "TUniformBuffer"));
    }

    void DestroyResource_RenderThread() override
    {
        Buffer.reset();
    }

    /**
     * Copy InData to the buffer
     * If InSize == 0 then copy sizeof(T)
     */
    void Copy(void* RESTRICT InData, uint64_t InSize = 0)
    {
        must(IsInRenderThread());

        if(InSize == 0)
            InSize = sizeof(T);

        void* RESTRICT Dst = Buffer->GetMappedData();

        if(!bUsePersistantMapping)
            Dst = Buffer->Map(ERSBufferMapMode::WriteOnly);

        memcpy(Dst, InData, InSize);

        if(!bUsePersistantMapping)
            Buffer->Unmap();
    }

    CRSBuffer* GetBuffer() const { return Buffer.get(); }
private:
	CRSBufferPtr Buffer;
};

}