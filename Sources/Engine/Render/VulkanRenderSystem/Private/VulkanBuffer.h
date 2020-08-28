#pragma once

#include "VulkanCore.h"
#include "VulkanDevice.h"
#include "Render/RenderSystem/Resources/Buffer.h"

class CVulkanDevice;

struct SVulkanBufferStagingBuffer
{
    CVulkanInternalStagingBuffer* StagingBuffer;
    ERSBufferMapMode MapMode;

    SVulkanBufferStagingBuffer() : StagingBuffer(nullptr),
        MapMode(ERSBufferMapMode::ReadOnly) {}
};

/**
 * An Vulkan buffer
 * Manage buffer within multiple frame inflight
 */
class CVulkanBuffer : public CVulkanDeviceResource,
    public CRSBuffer
{
public:
    CVulkanBuffer(CVulkanDevice& InDevice,
        const SRSBufferCreateInfo& InCreateInfo);
    virtual ~CVulkanBuffer();

    void* Map(ERSBufferMapMode InMapMode) override;
    void Unmap() override;
    
    void SetName(const char* InName) override;

    const vk::Buffer& GetBuffer() const { return Buffer; }
    void* GetMappedData() const override 
    { 
        /** GetMappedData() require Mapped hint */
        verify(CreateInfo.Hints & ERSMemoryHintFlagBits::Mapped);
        return AllocationInfo.pMappedData;
    }
private:
    vk::Buffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationInfo AllocationInfo;
    SVulkanBufferStagingBuffer CurrentStagingBuffer;
};