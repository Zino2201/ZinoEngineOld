#pragma once

#include "RenderCore.h"

/**
 * Buffer infos
 */
struct SBufferInfos
{
	uint32_t Size;
	EBufferUsageFlags Usage;
	EBufferMemoryUsage MemoryUsage;
	bool bUsePersistentMapping;

	SBufferInfos(const std::uint32_t& InSize, 
		const EBufferUsage& InUsage, const EBufferMemoryUsage& InMemUsage,
		const bool& bInUsePersistentMapping = false) : Size(InSize),
		Usage(InUsage), MemoryUsage(InMemUsage), bUsePersistentMapping(bInUsePersistentMapping) {}
};

/**
 * Buffer interface
 */
class IBuffer
{
public:
	IBuffer(const SBufferInfos& InInfos) {}
	virtual ~IBuffer() = default;

	/**
	 * Map the buffer
	 * Return a void* to be filled with memcpy
	 */
	virtual void* Map() = 0;

	/**
	 * Unmap the buffer
	 */
	virtual void Unmap() = 0;
};