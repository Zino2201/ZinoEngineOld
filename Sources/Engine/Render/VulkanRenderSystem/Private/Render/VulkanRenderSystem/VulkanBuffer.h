#pragma once

#include "VulkanCore.h"
#include "VulkanDevice.h"

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
    CVulkanBuffer(
        CVulkanDevice* InDevice,
		const ERSBufferUsage& InUsageFlags,
		const ERSMemoryUsage& InMemUsage,
		const uint64_t& InSize,
		const SRSResourceCreateInfo& InInfo);
    virtual ~CVulkanBuffer();

    void* Map(ERSBufferMapMode InMapMode);
    void Unmap();

    const uint64_t& GetSize() const { return Size; }
    const vk::Buffer& GetBuffer() const { return Buffer; }
private:
    vk::Buffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationInfo AllocationInfo;
    SVulkanBufferStagingBuffer CurrentStagingBuffer;
};