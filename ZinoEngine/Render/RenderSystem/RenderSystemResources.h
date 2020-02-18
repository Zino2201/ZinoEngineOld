#pragma once

#include "Render/RenderCore.h"
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

/**
 * An render system resource
 * Intrusive ptr
 */
class IRenderSystemResource : 
	public boost::intrusive_ref_counter<IRenderSystemResource, boost::thread_unsafe_counter>
{ 
public: 
	virtual ~IRenderSystemResource() {}

	virtual void Destroy() {}
};

/**
 * Buffer usage
 */
enum class EBufferUsage
{
	VertexBuffer = 1 << 0,
	IndexBuffer	= 1 << 1,
	TransferSrc = 1 << 2,
	TransferDst = 1 << 3,
	UniformBuffer = 1 << 4
};
DECLARE_FLAG_ENUM(EBufferUsage)

enum class EBufferMemoryUsage
{
	CpuOnly,
	GpuOnly,
	CpuToGpu,
	GpuToCpu
};

/** Textures */
enum class ETextureType
{
	Texture2D,
};

enum class ETextureViewType
{
	ShaderResource,
	DepthStencil,
};

enum class ETextureUsage
{
	TransferSrc = 1 << 0,
	TransferDst = 1 << 1,
	Sampled = 1 << 2,
	DepthStencil = 1 << 3
};
DECLARE_FLAG_ENUM(ETextureUsage)

enum class ETextureMemoryUsage
{
	CpuOnly,
	GpuOnly,
	CpuToGpu,
	GpuToCpu
};

/** Samplers */
enum class ESamplerFilter
{
	Linear,
	Nearest
};

enum class ESamplerAddressMode
{
	Repeat,
	Mirror,
	Clamp,
	Border,
};

/** Other */
enum class EComparisonOp
{
	Never,
	Always
};

/**
 * Buffer infos
 */
struct SRenderSystemBufferInfos
{
	std::string DebugName;
	uint64_t Size;
	EBufferUsageFlags Usage;
	EBufferMemoryUsage MemoryUsage;
	bool bUsePersistentMapping;

	SRenderSystemBufferInfos(const uint64_t& InSize, 
		const EBufferUsageFlags& InUsage, const EBufferMemoryUsage& InMemUsage,
		const bool& bInUsePersistentMapping = false,
		const std::string& InDebugName = "Buffer") : Size(InSize),
		Usage(InUsage), MemoryUsage(InMemUsage), bUsePersistentMapping(bInUsePersistentMapping),
		DebugName(InDebugName) {}
};

/**
 * Render System Buffer
 */
class CRenderSystemBuffer : public IRenderSystemResource
{
public:
	CRenderSystemBuffer(const SRenderSystemBufferInfos& InInfos) : Infos(InInfos) {}

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
	 * Copy buffer from src
	 */
	virtual void Copy(CRenderSystemBuffer* InSrc) = 0;

	/**
	 * Get mapped memory
	 */
	virtual void* GetMappedMemory() const = 0;

	virtual void Destroy() override = 0;

	const SRenderSystemBufferInfos& GetInfos() const { return Infos; }
protected:
	SRenderSystemBufferInfos Infos;
};

/** Texture */
struct SRenderSystemTextureInfo
{
	std::string DebugName;

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

	SRenderSystemTextureInfo(const ETextureType& InType,
		const EFormat& InFormat,
		const ETextureUsageFlags& InUsageFlags,
		const ETextureMemoryUsage& InMemoryUsage,
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth = 1,
		const uint32_t& InMipLevels = 1,
		const uint32_t& InArrayLayers = 1,
		const std::string& InDebugName = "Texture") : Type(InType), Format(InFormat),
		UsageFlags(InUsageFlags),
		MemoryUsage(InMemoryUsage),
		Width(InWidth), Height(InHeight), Depth(InDepth), MipLevels(InMipLevels),
		ArrayLayers(InArrayLayers), DebugName(InDebugName) {}
};

/**
 * A texture
 */
class CRenderSystemTexture : public IRenderSystemResource
{
public:
	CRenderSystemTexture(const SRenderSystemTextureInfo& InInfos) {}

	/** Copy texture from buffer */
	virtual void Copy(CRenderSystemBuffer* InSrc) = 0;

	virtual void Destroy() = 0;

	virtual const SRenderSystemTextureInfo& GetInfo() const = 0;
};

struct SRenderSystemTextureViewInfo
{
	CRenderSystemTexture* Texture;
	ETextureViewType ViewType;
	EFormat Format;
	uint32_t MipLevels;

	SRenderSystemTextureViewInfo(CRenderSystemTexture* InTexture, const ETextureViewType& InViewType,
		const EFormat& InFormat, const uint32_t& InMipLevels = 1) :
		Texture(InTexture), ViewType(InViewType), Format(InFormat), MipLevels(InMipLevels) {}
};

struct SRenderSystemSamplerInfo
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

	SRenderSystemSamplerInfo(
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

/**
 * An sampler
 */
class CRenderSystemSampler : public IRenderSystemResource
{
public:
	CRenderSystemSampler(const SRenderSystemSamplerInfo& InInfos) {}
};

using CRenderSystemSamplerPtr = boost::intrusive_ptr<CRenderSystemSampler>;

/**
 * A texture view
 */
class CRenderSystemTextureView : public IRenderSystemResource
{
public:
	CRenderSystemTextureView(const SRenderSystemTextureViewInfo& InInfos) {}

	void SetSampler(CRenderSystemSampler* InSampler) { Sampler = InSampler; }
	CRenderSystemSampler* GetSampler() const { return Sampler.get(); }
protected:
	CRenderSystemSamplerPtr Sampler;
};

/** Shader object */
class CRenderSystemShader : public IRenderSystemResource
{
public:
	CRenderSystemShader(void* InData,
		size_t InDataSize,
		const EShaderStage& InShaderStage) {}
};

/** Pipeline */

/**
 * Pipeline interface
 */
class CRenderSystemPipeline : public IRenderSystemResource
{
};

/**
 * Pipeline create infos
 */
struct SRenderSystemGraphicsPipelineInfos
{
	CRenderSystemShader* VertexShader;
	CRenderSystemShader* FragmentShader;
	SVertexInputBindingDescription BindingDescription;
	std::vector<SVertexInputAttributeDescription> AttributeDescriptions;
	std::vector<SShaderParameter> ShaderParameters;

	SRenderSystemGraphicsPipelineInfos(CRenderSystemShader* InVertexShader,
		CRenderSystemShader* InFragmentShader,
		const SVertexInputBindingDescription& InBindingDescription,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
		const std::vector<SShaderParameter>& InShaderParameters) : VertexShader(InVertexShader),
		FragmentShader(InFragmentShader), BindingDescription(InBindingDescription),
		AttributeDescriptions(InAttributeDescriptions), ShaderParameters(InShaderParameters) {}
};

/**
 * Graphics pipeline
 */
class IRenderSystemGraphicsPipeline : public CRenderSystemPipeline
{
public:
	IRenderSystemGraphicsPipeline(const SRenderSystemGraphicsPipelineInfos& InInfos) {}
};

/**
 * Uniform buffer infos
 */
struct SRenderSystemUniformBufferInfos
{
	/** Buffer size */
	uint64_t Size;

	/** Persistent mapping */
	bool bUsePersistentMapping;

	SRenderSystemUniformBufferInfos(const uint64_t& InSize,
		const bool& bInUsePersistentMapping = true) : Size(InSize), 
		bUsePersistentMapping(bInUsePersistentMapping) {}
};

/**
 * A uniform buffer
 */
class IRenderSystemUniformBuffer : public IRenderSystemResource
{
public:
	IRenderSystemUniformBuffer(const SRenderSystemUniformBufferInfos& InInfos) {}

	virtual void* Map() = 0;
	virtual void Unmap() = 0;
	virtual void* GetMappedMemory() const = 0;
	virtual CRenderSystemBuffer* GetBuffer() const = 0;
	virtual const SRenderSystemUniformBufferInfos& GetInfos() const = 0;
};

using IRenderSystemResourcePtr = boost::intrusive_ptr<IRenderSystemResource>;
using CRenderSystemBufferPtr = boost::intrusive_ptr<CRenderSystemBuffer>;
using CRenderSystemTexturePtr = boost::intrusive_ptr<CRenderSystemTexture>;
using CRenderSystemTextureViewPtr = boost::intrusive_ptr<CRenderSystemTextureView>;
using CRenderSystemShaderPtr = boost::intrusive_ptr<CRenderSystemShader>;
using IRenderSystemGraphicsPipelinePtr = boost::intrusive_ptr<IRenderSystemGraphicsPipeline>;
using IRenderSystemUniformBufferPtr = boost::intrusive_ptr<IRenderSystemUniformBuffer>;