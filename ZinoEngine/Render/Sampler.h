#pragma once

#include "Render/DeviceResource.h"

struct SSamplerInfo
{
	ESamplerFilter MagFilter;
	ESamplerFilter MinFilter;
	ESamplerFilter MipFilter;
	ESamplerAddressMode AddressModeU;
	ESamplerAddressMode AddressModeV;
	ESamplerAddressMode AddressModeW;

	/** Max anisotropy, 0 to disable */
	float MaxAnisotropy;
	EComparisonOp ComparisonOp;
	float MipLODBias;
	float MinLOD;
	float MaxLOD;

	SSamplerInfo(
		const ESamplerFilter& InMagFilter,
		const ESamplerFilter& InMinFilter,
		const ESamplerFilter& InMipFilter,
		const ESamplerAddressMode& InAddressModeU,
		const ESamplerAddressMode& InAddressModeV,
		const ESamplerAddressMode& InAddressModeW,
		const float& InMaxAnisotropy,
		const EComparisonOp& InComparisonOp,
		const float& InMipLODBias,
		const float& InMinLOD,
		const float& InMaxLOD) :
		MagFilter(InMagFilter), MinFilter(InMinFilter), MipFilter(InMipFilter),
		AddressModeU(InAddressModeU), AddressModeV(InAddressModeV), AddressModeW(InAddressModeW),
		MaxAnisotropy(InMaxAnisotropy), ComparisonOp(InComparisonOp),
		MipLODBias(InMipLODBias), MinLOD(InMinLOD), MaxLOD(InMaxLOD) {}
};

class ISampler : public IDeviceResource
{
public:
	ISampler(const SSamplerInfo& InInfos) {}
	virtual ~ISampler() = default;
};