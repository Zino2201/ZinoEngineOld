#pragma once

#include "RenderCore.h"

class IBuffer;

/**
 * Uniform buffer infos
 */
struct SUniformBufferInfos
{
	/** Buffer size */
	uint64_t Size;

	/** Persistent mapping */
	bool bUsePersistentMapping;

	SUniformBufferInfos(const uint64_t& InSize,
		const bool& bInUsePersistentMapping = true) : Size(InSize), 
		bUsePersistentMapping(bInUsePersistentMapping) {}
};

/**
 * A uniform buffer
 */
class IUniformBuffer
{
public:
	IUniformBuffer(const SUniformBufferInfos& InInfos) {}
	virtual ~IUniformBuffer() = default;

	virtual void* Map() = 0;
	virtual void Unmap() = 0;
	virtual void* GetMappedMemory() const = 0;
	virtual IBuffer* GetBuffer() const = 0;
};