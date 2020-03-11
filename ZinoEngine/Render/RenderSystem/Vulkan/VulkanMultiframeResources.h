#pragma once

#include "VulkanCore.h"
#include "VulkanBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"

/**
 * Resources that extend in multiple frames
 */

/**
 * A back-buffered resource
 */
class CVulkanMultiframeBuffer
{
public:
    CVulkanMultiframeBuffer(const SRenderSystemBufferInfos& InInfos);

    void Destroy()
    {
        for(const auto& Buffer : Buffers)
		    Buffer->Destroy();
    }

    void Copy(CRenderSystemBuffer* InSrc)
    {
        Buffers[CurrentBuffer]->Copy(InSrc);
    }

    void* Map()
    {
        return Buffers[CurrentBuffer]->Map();
    }

	void Unmap()
	{
		return Buffers[CurrentBuffer]->Unmap();
    }

    CVulkanBuffer* GetBuffer() const 
    { 
        return static_cast<CVulkanBuffer*>(Buffers[CurrentBuffer].get());
    }
private:
    void OnFrameCompletedDelegate();
protected:
    std::vector<CRenderSystemBufferPtr> Buffers;

    /** Buffer used for cpu */
    uint64_t CurrentBuffer;
};