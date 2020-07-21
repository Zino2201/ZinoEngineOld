#pragma once

namespace ZE
{

#include "Resource.h"

/**
 * Buffer usage flags
 */
enum class ERSBufferUsage
{
    None = 1 << 0,

    VertexBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    UniformBuffer = 1 << 3
};
DECLARE_FLAG_ENUM(ERSBufferUsage);

/**
 * Map mode buffer
 */
enum class ERSBufferMapMode
{
    /** Read only */
    ReadOnly,

    /** Will be written */
    WriteOnly
};

struct SRSBufferCreateInfo
{
    ERSBufferUsage UsageFlags;
    ERSMemoryUsage MemoryUsage;
    uint64_t Size;
    const char* DebugName;

    SRSBufferCreateInfo(const ERSBufferUsage& InUsageFlags,
        const ERSMemoryUsage& InMemoryUsage,
        const uint64_t& InSize,
        const char* InDebugName = "Buffer") : UsageFlags(InUsageFlags),
        MemoryUsage(InMemoryUsage), Size(InSize), DebugName(InDebugName) {}
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
    virtual void* GetMappedData() const { return nullptr; }
protected:
	SRSBufferCreateInfo CreateInfo;
};

namespace RSUtils
{
    RENDERSYSTEM_API void Copy(const void* Src, CRSBuffer* Dst);
};

}
