#pragma once

#include "DeviceResource.h"

class ITexture;

/**
 * Buffer infos
 */
struct SBufferInfos
{
	uint64_t Size;
	EBufferUsageFlags Usage;
	EBufferMemoryUsage MemoryUsage;
	bool bUsePersistentMapping;

	SBufferInfos(const uint64_t& InSize, 
		const EBufferUsageFlags& InUsage, const EBufferMemoryUsage& InMemUsage,
		const bool& bInUsePersistentMapping = false) : Size(InSize),
		Usage(InUsage), MemoryUsage(InMemUsage), bUsePersistentMapping(bInUsePersistentMapping) {}
};

/**
 * Buffer interface
 */
class IBuffer : public IDeviceResource
{
public:
	IBuffer(const SBufferInfos& InInfos) : Infos(InInfos) {}
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

	/**
	 * Copy buffer to specified dest
	 */
	virtual void Copy(IBuffer* InDst) = 0;

	/* 
	 * Copy to texture
	 */
	virtual void Copy(ITexture* InDst) = 0;

	/**
	 * Get mapped memory
	 */
	virtual void* GetMappedMemory() const = 0;

	const SBufferInfos& GetInfos() const { return Infos; }
protected:
	SBufferInfos Infos;
};