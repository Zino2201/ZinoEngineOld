#pragma once

#include "EngineCore.h"
#include <any>
#include <algorithm>
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"

namespace ZE
{

/**
 * Wrapper around a CRSBuffer
 */
template<typename T, bool bUsePersistantMapping = true>
struct TUniformBuffer
{
    TUniformBuffer()
    {
        ERSMemoryUsage MemoryUsage = ERSMemoryUsage::HostVisible;
        if constexpr(bUsePersistantMapping)
            MemoryUsage |= ERSMemoryUsage::UsePersistentMapping;

        Buffer = GRenderSystem->CreateBuffer(
            ERSBufferUsage::UniformBuffer,
            MemoryUsage,
            sizeof(T),
            SRSResourceCreateInfo(nullptr, "TUniformBuffer"));
    }

    ~TUniformBuffer()
    {
        Buffer.reset();
    }

    /**
     * Copy InData to the buffer
     * If InSize == 0 then copy sizeof(T)
     */
    void Copy(const void* RESTRICT InData, uint64_t InSize = 0)
    {
        if(InSize == 0)
            InSize = sizeof(T);

        void* RESTRICT Dst = Buffer->GetMappedData();

        if constexpr(!bUsePersistantMapping)
            Dst = Buffer->Map(ERSBufferMapMode::WriteOnly);

        memcpy(Dst, InData, InSize);

        if constexpr(!bUsePersistantMapping)
            Buffer->Unmap();
    }

	void Copy(const T& InData)
	{
		Copy(reinterpret_cast<const void*>(&InData));
	}

    CRSBuffer* GetBuffer() const { return Buffer.get(); }
private:
	CRSBufferPtr Buffer;
};

}