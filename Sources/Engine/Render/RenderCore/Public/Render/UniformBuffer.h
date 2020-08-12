#pragma once

#include "EngineCore.h"
#include <any>
#include <algorithm>
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/Resources/Buffer.h"

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
        Buffer = GRenderSystem->CreateBuffer({
            ERSBufferUsageFlagBits::UniformBuffer,
            ERSMemoryUsage::HostVisible,
            bUsePersistantMapping ? ERSMemoryHintFlagBits::Mapped : ERSMemoryHintFlagBits::None,
            sizeof(T) });
        Buffer->SetName("TUniformBuffer");

    }

    /**
     * Copy InData to the buffer
     * If InSize == 0 then copy sizeof(T)
     */
    void Copy(const void* ZE_RESTRICT InData, uint64_t InSize = 0)
    {
        if (InSize == 0)
            InSize = sizeof(T);

        void* ZE_RESTRICT Dst = Buffer->GetMappedData();

        if constexpr(!bUsePersistantMapping)
            Dst = Buffer->Map(ERSBufferMapMode::WriteOnly);

        memcpy(Dst, InData, InSize);

        if constexpr(!bUsePersistantMapping)
            Buffer->Unmap();
    }

	ZE_FORCEINLINE void Copy(const T& InData)
	{
		Copy(reinterpret_cast<const void*>(&InData));
	}

    ZE_FORCEINLINE CRSBuffer* GetBuffer() const { return Buffer.get(); }
private:
	CRSBufferPtr Buffer;
};

}