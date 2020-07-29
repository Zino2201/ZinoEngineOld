#pragma once

namespace ZE
{

#include "Resource.h"

/**
 * Buffer usage flags
 */
enum class ERSBufferUsageFlagBits
{
    None = 0,

    VertexBuffer = 1 << 0,
    IndexBuffer = 1 << 1,
    UniformBuffer = 1 << 2
};
ENABLE_FLAG_ENUMS(ERSBufferUsageFlagBits, ERSBufferUsageFlags);

/**
 * Map mode buffer
 */
enum class ERSBufferMapMode
{
    /** Read only, will  */
    ReadOnly,

    /** Will be written */
    WriteOnly,
};

struct SRSBufferCreateInfo
{
    ERSBufferUsageFlags UsageFlags;
    ERSMemoryUsage MemoryUsage;
    ERSMemoryHintFlagBits Hints;
    uint64_t Size;
    const char* DebugName;

    SRSBufferCreateInfo(const ERSBufferUsageFlags& InUsageFlags,
        const ERSMemoryUsage& InMemoryUsage,
        const ERSMemoryHintFlagBits& InHints,
        const uint64_t& InSize,
        const char* InDebugName = "Buffer") : UsageFlags(InUsageFlags),
        MemoryUsage(InMemoryUsage), Hints(InHints), Size(InSize), DebugName(InDebugName) {}
};

/**
 * A buffer
 */
class CRSBuffer : public CRSResource
{
public:
	CRSBuffer(const SRSBufferCreateInfo& InCreateInfo) : CreateInfo(InCreateInfo) {}

	virtual void* Map(ERSBufferMapMode InMapMode) { return nullptr; }
	virtual void Unmap() {}

	FORCEINLINE const SRSBufferCreateInfo& GetCreateInfo() const { return CreateInfo; }

    /**
     * Get mapped buffer data (only valid when using Mapped hint !)
     */
    virtual void* GetMappedData() const { return nullptr; }
protected:
	SRSBufferCreateInfo CreateInfo;
};

namespace RSUtils
{
    RENDERSYSTEM_API void Copy(const void* Src, CRSBuffer* Dst);
};

}
