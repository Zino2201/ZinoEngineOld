#pragma once

#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"

/**
 * Templated version of RenderSystemUniformBuffer
 */
template<typename T, bool bUsePersistantMapping = true>
class TUniformBuffer : public CRenderResource
{
public:
	virtual void InitRenderThread() override
	{
		UniformBuffer = g_Engine->GetRenderSystem()
			->CreateUniformBuffer(SRenderSystemUniformBufferInfos(sizeof(T), bUsePersistantMapping));
	}

	virtual void DestroyRenderThread() override
	{
		UniformBuffer->Destroy();
	}

	/**
	 * Copy data into uniform buffer, if size == 0 use sizeof(T)
	 */
	void Copy(void* InData, uint64_t InSize = 0)
	{
		must(IsInRenderThread());

		if(InSize == 0)
			InSize = sizeof(T);

		void* Dst = UniformBuffer->GetInfos().bUsePersistentMapping ?
			UniformBuffer->GetMappedMemory() : UniformBuffer->Map();

		memcpy(Dst, InData, InSize);

		if(!UniformBuffer->GetInfos().bUsePersistentMapping)
			UniformBuffer->Unmap();
	}

	IRenderSystemUniformBuffer* GetUniformBuffer() const { return UniformBuffer.get(); }
private:
	IRenderSystemUniformBufferPtr UniformBuffer;
};