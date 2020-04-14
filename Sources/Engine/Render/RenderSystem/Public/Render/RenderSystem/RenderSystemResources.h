#pragma once

#include "EngineCore.h"
#include "Render/RenderCore.h"

namespace ZE
{

/**
 * Buffer usage flags
 */
enum class ERSMemoryUsage
{
    /** Memory that will be only accessed by the GPU */
    DeviceLocal = 1 << 0,

    /** Memory that can be mapped by the CPU */
    HostVisible = 1 << 1,

    /** Memory stored in host memory */
    HostOnly = 1 << 2
};
DECLARE_FLAG_ENUM(ERSMemoryUsage);

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

/**
 * Texture usage
 */
enum class ERSTextureUsage
{
    None = 1 << 0,
    Sampled = 1 << 1,
    RenderTarget = 1 << 2,
    DepthStencil = 1 << 3
};
DECLARE_FLAG_ENUM(ERSTextureUsage);

/**
 * Texture type
 */
enum class ERSTextureType
{
    Tex1D,
    Tex2D,
    Tex3D
};


/** Fwds */
class CRSTexture;

/**
 * Required create infos for every resources
 */
struct RENDERSYSTEM_API SRSResourceCreateInfo
{
    /** Debug name */
    const char* DebugName;

    /** Initial data to copy (required for static buffers/textures) */
    const void* InitialData;

    SRSResourceCreateInfo() : DebugName("Unknown"), InitialData(nullptr) {}
    SRSResourceCreateInfo(const void* InInitialData) : DebugName("Unknown"), InitialData(InInitialData) {}
    SRSResourceCreateInfo(const void* InInitialData,
        const char* InName) : DebugName(InName), InitialData(InInitialData) {}
};

/**
 * Base interface for a render system resource
 */
class RENDERSYSTEM_API CRSResource :
    public boost::intrusive_ref_counter<CRSResource, boost::thread_unsafe_counter>
{
public:
    CRSResource(const SRSResourceCreateInfo& InCreateInfo) :
        CreateInfo(InCreateInfo) {}

    virtual ~CRSResource() {}

    void Destroy();
protected:
    SRSResourceCreateInfo CreateInfo;
};

/**
 * An surface
 * Contains its own swap chain
 */
class CRSSurface : public CRSResource
{
public:
    CRSSurface(void* InWindowHandle, const uint32_t& InWidth, const uint32_t& InHeight,
        const SRSResourceCreateInfo& InInfo) : CRSResource(InInfo),
        WindowHandle(InWindowHandle),
        Width(InWidth), Height(InHeight) {}

    virtual EFormat GetSwapChainFormat() const = 0;
    virtual CRSTexture* GetBackbufferTexture() const = 0;
protected:
    void* WindowHandle;
	uint32_t Width;
	uint32_t Height;
};

/**
 * Buffer usage flags
 */
enum class ERSBufferUsage
{
    VertexBuffer,
    IndexBuffer,
    UniformBuffer
};
DECLARE_FLAG_ENUM(ERSBufferUsage);

/**
 * A buffer
 */
class CRSBuffer : public CRSResource
{
public:
	CRSBuffer(
        const ERSBufferUsage& InUsageFlags,
        const ERSMemoryUsage& InMemUsage,
        const uint64_t& InSize, 
		const SRSResourceCreateInfo& InInfo) : CRSResource(InInfo),
		Size(InSize),
		MemoryUsage(InMemUsage) {}

	virtual void* Map(ERSBufferMapMode InMapMode) { return nullptr; }
	virtual void Unmap() {}

	const uint64_t& GetSize() const { return Size; }
protected:
	uint64_t Size;
	ERSMemoryUsage MemoryUsage;
};

/**
 * A texture
 * Type is determined by ERSTextureType
 */
class CRSTexture : public CRSResource 
{
public:
	CRSTexture(
        const ERSTextureType& InTextureType,
        const ERSTextureUsage& InTextureUsage,
		const ERSMemoryUsage& InMemoryUsage,
        const EFormat& InFormat, 
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth,
        const uint32_t& InArraySize,
		const uint32_t& InMipLevels,
		const ESampleCount& InSampleCount,
        const SRSResourceCreateInfo& InInfo) : 
        CRSResource(InInfo), 
        TextureUsage(InTextureUsage), MemoryUsage(InMemoryUsage),
        Format(InFormat), Width(InWidth), Height(InHeight){}

    const uint32_t& GetWidth() const { return Width; }
    const uint32_t& GetHeight() const { return Height; }
    const EFormat& GetFormat() const { return Format; }
protected:
    EFormat Format;
    ERSTextureUsage TextureUsage;
    ERSMemoryUsage MemoryUsage;
	uint32_t Width;
	uint32_t Height;
};

#pragma region Render Pass

/** Render pass */
enum class ERSRenderPassAttachmentLoadOp
{
    Clear,
    Load,
    DontCare
};

enum class ERSRenderPassAttachmentStoreOp
{
    Store,
    DontCare
};

/**
 * Layout of the attachment
 */
enum class ERSRenderPassAttachmentLayout
{
	/** Don't care */
	Undefined,

	/** Used for color attachment */
	ColorAttachment,

    DepthStencilAttachment,

	/** Used for present */
	Present,
};

/**
 * Describe an attachment
 */
struct SRSRenderPassAttachment
{
	EFormat Format;
	ESampleCount SampleCount;
	ERSRenderPassAttachmentLoadOp Load;
	ERSRenderPassAttachmentStoreOp Store;
	ERSRenderPassAttachmentLayout InitialLayout;
	ERSRenderPassAttachmentLayout FinalLayout;

	bool operator==(const SRSRenderPassAttachment& InOther) const
	{
		return Format == InOther.Format &&
			SampleCount == InOther.SampleCount &&
			Load == InOther.Load &&
			Store == InOther.Store &&
			InitialLayout == InOther.InitialLayout &&
			FinalLayout == InOther.FinalLayout;
	}
};

/*
 * A subpass attachment reference
 */
struct SRSRenderPassSubpassAttachmentRef
{
	/** Attachment idx */
	uint32_t Index;

	/** Layout that the attachment should have when beginning the subpass */
	ERSRenderPassAttachmentLayout Layout;

	bool operator==(const SRSRenderPassSubpassAttachmentRef& InOther) const
	{
		return Index == InOther.Index &&
			Layout == InOther.Layout;
	}
};

/**
 * Describe a subpass
 */
struct SRSRenderPassSubpass
{
    std::vector<SRSRenderPassSubpassAttachmentRef> ColorAttachmentRefs;
    std::vector<SRSRenderPassSubpassAttachmentRef> DepthAttachmentRefs;

    bool operator==(const SRSRenderPassSubpass& InOther) const
    {
        return ColorAttachmentRefs == InOther.ColorAttachmentRefs;
    }
};

/**
 * Represents a render pass
 * 
 * A render pass can contains subpasses that depend from specific attachments
 */
struct SRSRenderPass
{
    std::vector<SRSRenderPassAttachment> ColorAttachments;
    std::vector<SRSRenderPassAttachment> DepthAttachments;
    std::vector<SRSRenderPassSubpass> Subpasses;

    bool operator==(const SRSRenderPass& InOther) const
    {
        return ColorAttachments == InOther.ColorAttachments &&
            Subpasses == InOther.Subpasses;
    }
};

struct SRSRenderPassAttachmentHash
{
	std::size_t operator()(const SRSRenderPassAttachment& InAttachment) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine(Seed, InAttachment.Format);
		HashCombine(Seed, InAttachment.Load);
		HashCombine(Seed, InAttachment.Store);
		HashCombine(Seed, InAttachment.InitialLayout);
		HashCombine(Seed, InAttachment.FinalLayout);

		return Seed;
	}
};

struct SRSRenderPassSubpassAttachmentRefHash
{
	std::size_t operator()(const SRSRenderPassSubpassAttachmentRef& InRef) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine(Seed, InRef.Index);
		HashCombine(Seed, InRef.Layout);

		return Seed;
	}
};

struct SRSRenderPassSubpassHash
{
	std::size_t operator()(const SRSRenderPassSubpass& InSubpass) const noexcept
	{
		std::size_t Seed = 0;

		for (const auto& Ref : InSubpass.ColorAttachmentRefs)
		{
			HashCombine<SRSRenderPassSubpassAttachmentRef,
				SRSRenderPassSubpassAttachmentRefHash>(Seed, Ref);
		}

		return Seed;
	}
};

struct SRSRenderPassHash
{
	std::size_t operator()(const SRSRenderPass& InPass) const noexcept
	{
		std::size_t Seed = 0;

		for (const auto& Attachment : InPass.ColorAttachments)
		{
			HashCombine<SRSRenderPassAttachment,
				SRSRenderPassAttachmentHash>(Seed, Attachment);
		}

		for (const auto& Subpass : InPass.Subpasses)
		{
			HashCombine<SRSRenderPassSubpass,
				SRSRenderPassSubpassHash>(Seed, Subpass);
		}

		return Seed;
	}
};

#pragma endregion

constexpr uint32_t GMaxRenderTargetPerFramebuffer = 8;

/**
 * Details about a framebuffer (collection of render targets)
 * Used by BeginRenderPass
 */
struct SRSFramebuffer
{
    /** Color render targets */
    CRSTexture* ColorRTs[GMaxRenderTargetPerFramebuffer];

    /** Depth render targets */
    CRSTexture* DepthRTs[GMaxRenderTargetPerFramebuffer];

	bool operator==(const SRSFramebuffer& InFramebuffer) const
	{
		for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
		{
			if (ColorRTs[i] != InFramebuffer.ColorRTs[i])
                return false;

			if (DepthRTs[i] != InFramebuffer.DepthRTs[i])
				return false;
		}

        return true;
	}
};

struct SRSFramebufferHash
{
	std::size_t operator()(const SRSFramebuffer& InFramebuffer) const noexcept
	{
		std::size_t Seed = 0;

		for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
		{
			if (!InFramebuffer.ColorRTs[i])
				continue;

			HashCombine(Seed, InFramebuffer.ColorRTs[i]);

			if (!InFramebuffer.DepthRTs[i])
				continue;

			HashCombine(Seed, InFramebuffer.DepthRTs[i]);
		}

		return Seed;
	}
};

/**
 * A handle to a shader
 */
class CRSShader : public CRSResource
{
public:
    CRSShader(const EShaderStage& InStage, 
        const uint64_t& InBytecodeSize,
        const void* InBytecode,
        const SShaderParameterMap& InParameterMap,
        const SRSResourceCreateInfo& InCreateInfo) :
       CRSResource(InCreateInfo), Stage(InStage), ParameterMap(InParameterMap) {}

    const SShaderParameterMap& GetShaderParameterMap() const { return ParameterMap; }
protected:
    EShaderStage Stage;
    SShaderParameterMap ParameterMap;
};

/**
 * Comparison op
 */
/** Other */
enum class ERSComparisonOp
{
	Never,
	Always,
	Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual
};

/**
 * Pipeline objects and states
 */
#pragma region Pipeline & States

/** Vertex input rate */
enum class EVertexInputRate
{
	Vertex,
	Instance
};

struct SVertexInputBindingDescription
{
	uint32_t Binding;
	uint32_t Stride;
	EVertexInputRate InputRate;

	SVertexInputBindingDescription(const uint32_t& InBinding,
		const uint32_t& InStride, const EVertexInputRate& InInputRate) :
		Binding(InBinding), Stride(InStride), InputRate(InInputRate) {}
};

/**
 * Vertex input description
 */
struct SVertexInputAttributeDescription
{
	uint32_t Binding;
	uint32_t Location;
	EFormat Format;
	uint32_t Offset;

	SVertexInputAttributeDescription(const uint32_t& InBinding,
		const uint32_t& InLocation, const EFormat& InFormat, const uint32_t& InOffset) :
		Binding(InBinding), Location(InLocation), Format(InFormat), Offset(InOffset) {}
};

/** BLEND STATE */
enum class EBlendFactor
{
    One,
    Zero,
    SrcAlpha,
    OneMinusSrcAlpha
};

enum class EBlendOp
{
    Add,
    Substract,
    ReverseSubstract,
    Min,
    Max
};

/**
 * Blend state
 */
struct SRSBlendState
{
    bool bEnableBlend;
    EBlendFactor SrcColor;
    EBlendFactor DstColor;
    EBlendOp ColorOp;
    EBlendFactor SrcAlpha;
    EBlendFactor DstAlpha;
    EBlendOp AlphaOp;

    /**
     * Default state ctor
     */
    SRSBlendState(
        const bool& bInEnableBlending = false,
        const EBlendFactor& InSrcColor = EBlendFactor::One,
        const EBlendFactor& InDstColor = EBlendFactor::Zero,
        const EBlendOp& InColorOp = EBlendOp::Add,
        const EBlendFactor& InSrcAlpha = EBlendFactor::One,
        const EBlendFactor& InDstAlpha = EBlendFactor::Zero,
        const EBlendOp& InAlphaOp = EBlendOp::Add)
        : bEnableBlend(bInEnableBlending), 
            SrcColor(InSrcColor), DstColor(InDstColor), ColorOp(InColorOp),
            SrcAlpha(InSrcAlpha), DstAlpha(InDstAlpha), AlphaOp(InAlphaOp) {}
};

/** DEPTH AND STENCIL STATE */
enum class EStencilOp
{
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap
};

/**
 * Depth stencil op
 */
struct SRSDepthStencilOp
{
    EStencilOp FailOp;
    EStencilOp PassOp;
    EStencilOp DepthFailOp;
    ERSComparisonOp CompareOp;

	SRSDepthStencilOp() :
		FailOp(EStencilOp::Keep),
		PassOp(EStencilOp::Keep),
		DepthFailOp(EStencilOp::Keep),
		CompareOp(ERSComparisonOp::Never) {}
};

/**
 * Depth stencil state
 */
struct SRSDepthStencilState
{
    bool bEnableDepthTest;
    bool bEnableDepthWrite;
    ERSComparisonOp DepthCompareOp;
	bool bDepthBoundsTestEnable;
	bool bStencilTestEnable;
	SRSDepthStencilOp FrontFace;
	SRSDepthStencilOp BackFace;

	SRSDepthStencilState(
		const bool& bInDepthTestEnable = false,
		const bool& bInDepthWriteEnable = false,
		const ERSComparisonOp& InComparaisonOp = ERSComparisonOp::Never,
		const bool bInDepthBoundsTestEnable = false,
		const bool bInStencilTestEnable = false,
		const SRSDepthStencilOp& InFrontFace = SRSDepthStencilOp(),
		const SRSDepthStencilOp& InBackFace = SRSDepthStencilOp()) :
		bEnableDepthTest(bInDepthTestEnable), 
		bStencilTestEnable(bInStencilTestEnable), 
        bEnableDepthWrite(bInDepthWriteEnable),
		DepthCompareOp(InComparaisonOp),
		bDepthBoundsTestEnable(bInDepthBoundsTestEnable),
		FrontFace(InFrontFace), BackFace(InBackFace) {}
};

/** RENDER STATES */
enum class EPolygonMode
{
    Fill,
    Line,
    Point
};

enum class ECullMode
{
    None,
    Back,
    Front,
    FrontAndBack
};

enum class EFrontFace
{
    Clockwise,
    CounterClockwise
};

/** Rasterizer state */
struct SRSRasterizerState
{
    EPolygonMode PolygonMode;
    ECullMode CullMode;
    EFrontFace FrontFace;
    bool bEnableDepthClamp;
    bool bEnableRasterizerDiscard;

    SRSRasterizerState(
        const EPolygonMode& InPolygonMode = EPolygonMode::Fill,
        const ECullMode& InCullMode = ECullMode::Back,
        const EFrontFace& InFrontFace = EFrontFace::CounterClockwise,
        const bool& bInEnableDepthClamp = false,
        const bool& bInEnableRasterizerDiscard = false) :
        PolygonMode(InPolygonMode), CullMode(InCullMode), FrontFace(InFrontFace),
        bEnableDepthClamp(bInEnableDepthClamp), 
        bEnableRasterizerDiscard(bInEnableRasterizerDiscard) {}
};

/**
 * An pipeline shader stage
 */
struct SRSPipelineShaderStage
{
    EShaderStage Stage;
    CRSShader* Shader;
    const char* EntryPoint;

    SRSPipelineShaderStage(
		const EShaderStage& InStage,
	    CRSShader* InShader,
        const char* InEntryPoint) : 
        Stage(InStage), Shader(InShader), EntryPoint(InEntryPoint) {}
};

/**
 * A pipeline object
 */
class CRSPipeline : public CRSResource
{
public:
    CRSPipeline(
        const std::vector<SRSPipelineShaderStage>& InShaderStages,
        const SRSResourceCreateInfo& InCreateInfo) : CRSResource(InCreateInfo) {}
};

/**
 * A graphics pipeline used for graphics operations
 */
class CRSGraphicsPipeline : public CRSPipeline
{
public:
    CRSGraphicsPipeline(
        const std::vector<SRSPipelineShaderStage>& InShaderStages,
        const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
        const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
        const SRSRenderPass& InRenderPass,
        const SRSBlendState& InBlendState,
        const SRSRasterizerState& InRasterizerState,
        const SRSDepthStencilState& InDepthStencilState,
        const SRSResourceCreateInfo& InCreateInfo) :
        CRSPipeline(InShaderStages, InCreateInfo) {}
};

#pragma endregion

/** Intrusive ptrs */
using CRSSurfacePtr = boost::intrusive_ptr<CRSSurface>;
using CRSBufferPtr = boost::intrusive_ptr<CRSBuffer>;
using CRSTexturePtr = boost::intrusive_ptr<CRSTexture>;
using CRSShaderPtr = boost::intrusive_ptr<CRSShader>;
using CRSGraphicsPipelinePtr = boost::intrusive_ptr<CRSGraphicsPipeline>;

}