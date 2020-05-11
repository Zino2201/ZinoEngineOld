#pragma once

#include "EngineCore.h"
#include <any>
#include "RenderThreadResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"

namespace ZE
{

/**
 * Wrapper around a CRSBuffer
 */
template<typename T, bool bUsePersistantMapping = false>
struct TUniformBuffer : public CRenderThreadResource
{
    void InitResource_RenderThread() override
    {
        Buffer = GRenderSystem->CreateBuffer(
            ERSBufferUsage::UniformBuffer,
            ERSMemoryUsage::HostVisible,
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
    void Copy(const std::any& InData, uint64_t InSize = 0)
    {
        must(IsInRenderThread());

        if(InSize == 0)
            InSize = sizeof(T);

        void* Dst = Buffer->GetMappedData();

        if(!bUsePersistantMapping)
            Dst = Buffer->Map(ERSBufferMapMode::WriteOnly);

        memcpy(Dst, InData, InSize);

        if(!bUsePersistantMapping)
            Buffer->Unmap();
    }
private:
	CRSBufferPtr Buffer;
};

}