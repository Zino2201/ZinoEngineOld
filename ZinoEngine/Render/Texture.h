#pragma once

#include "RenderCore.h"
#include "Render/DeviceResource.h"

struct STextureInfo
{
	/** Texture type */
	ETextureType Type;

	/** Texture format */
	EFormat Format;

	/** Usage flags */
	ETextureUsageFlags UsageFlags;

	/** Memory usage */
	ETextureMemoryUsage MemoryUsage;

	/** Texture width */
	uint32_t Width;
	
	/** Texture height */
	uint32_t Height;

	/** Texture depth, should be 1 if not used */
	uint32_t Depth;

	/** Mip levels */
	uint32_t MipLevels;

	/** Array layers */
	uint32_t ArrayLayers;
	
	STextureInfo(const ETextureType& InType,
		const EFormat& InFormat,
		const ETextureUsageFlags& InUsageFlags,
		const ETextureMemoryUsage& InMemoryUsage,
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth = 1,
		const uint32_t& InMipLevels = 1,
		const uint32_t& InArrayLayers = 1) : Type(InType), Format(InFormat),
		UsageFlags(InUsageFlags),
		MemoryUsage(InMemoryUsage),
		Width(InWidth), Height(InHeight), Depth(InDepth), MipLevels(InMipLevels),
		ArrayLayers(InArrayLayers) {}
};

class ITexture : public IDeviceResource
{
public:
	ITexture(const STextureInfo& InInfos) {}
	virtual ~ITexture() = default;
};