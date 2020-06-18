#pragma once

#include "EngineCore.h"
#include "Shader/ShaderCore.h"
#include <array>

namespace ZE
{

/**
 * Buffer usage flags
 */
enum class ERSMemoryUsage
{
    /** MEMORY TYPES */

    /** Memory that will be only accessed by the GPU */
    DeviceLocal = 1 << 0,

    /** Memory that can be mapped by the CPU */
    HostVisible = 1 << 1,

    /** Memory stored in host memory */
    HostOnly = 1 << 2,

    /** FLAGS */

    UsePersistentMapping = 1 << 3,
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

    virtual void Resize(const uint32_t& InWidth, const uint32_t& InHeight) = 0;
    virtual EFormat GetSwapChainFormat() const = 0;
    virtual CRSTexture* GetBackbufferTexture() = 0;
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
    virtual void* GetMappedData() const { return nullptr; }
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
        CRSResource(InInfo), Type(InTextureType),
        TextureUsage(InTextureUsage), MemoryUsage(InMemoryUsage),
        Format(InFormat), Width(InWidth), Height(InHeight){}

    const uint32_t& GetWidth() const { return Width; }
    const uint32_t& GetHeight() const { return Height; }
    const EFormat& GetFormat() const { return Format; }
    const ERSTextureUsage& GetTextureUsage() const { return TextureUsage; }
    const ERSTextureType& GetType() const { return Type; }
protected:
    ERSTextureType Type;
    EFormat Format;
    ERSTextureUsage TextureUsage;
    ERSMemoryUsage MemoryUsage;
	uint32_t Width;
	uint32_t Height;
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

/** SAMPLER **/

enum class ERSFilter
{
    Nearest,
    Linear,
};

enum class ERSSamplerAddressMode
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class ERSBorderColor
{
    FloatTransparentBlack,
    IntTransparentBlack,
    FloatOpaqueBlack,
    IntOpaqueBlack,
    FloatOpaqueWhite,
    IntOpaqueWhite,
};

/**
 * Create info for a sampler
 */
struct SRSSamplerCreateInfo
{
    ERSFilter MinFilter;
    ERSFilter MagFilter;
    ERSFilter MipMapMode;
    ERSSamplerAddressMode AddressModeU;
    ERSSamplerAddressMode AddressModeV;
    ERSSamplerAddressMode AddressModeW;
    float MipLodBias;
    ERSComparisonOp CompareOp;
    bool bAnistropyEnable;
    float MaxAnistropy;
    float MinLOD;
    float MaxLOD;

    SRSSamplerCreateInfo(
        const ERSFilter& InMinFilter = ERSFilter::Linear,
        const ERSFilter& InMagFilter = ERSFilter::Linear,
        const ERSFilter& InMipMapMode = ERSFilter::Linear,
        const ERSSamplerAddressMode& InAddressModeU = ERSSamplerAddressMode::Repeat,
        const ERSSamplerAddressMode& InAddressModeV = ERSSamplerAddressMode::Repeat,
        const ERSSamplerAddressMode& InAddressModeW = ERSSamplerAddressMode::Repeat,
        const float& InMipLodBias = 0.f,
        const ERSComparisonOp& InCompareOp = ERSComparisonOp::Never,
        const bool& bInAnistropyEnable = false,
        const float& InMaxAnistropy = 0.f,
        const float& InMinLOD = 0.f,
        const float& InMaxLOD = 0.f) : MinFilter(InMinFilter), MagFilter(InMagFilter),
            MipMapMode(InMipMapMode), AddressModeU(InAddressModeU), AddressModeV(InAddressModeV),
            AddressModeW(InAddressModeW), MipLodBias(InMipLodBias), CompareOp(InCompareOp),
            bAnistropyEnable(bInAnistropyEnable), MaxAnistropy(InMaxAnistropy), MinLOD(InMinLOD),
            MaxLOD(InMaxLOD){}
};

/**
 * A sampler
 */
class CRSSampler : public CRSResource
{
public:
    CRSSampler(const SRSSamplerCreateInfo& InSamplerCreateInfo) : CRSResource({}) {}
};

#pragma region Render Pass

/**
 * A viewport
 */
struct SViewport
{
	Math::SRect2D Rect;
	float MinDepth;
	float MaxDepth;
};

/** Index format */
enum class EIndexFormat
{
	Uint16,
	Uint32
};

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

    /** Used for depth attachment */
    DepthStencilAttachment,

    /** Optimal for shader read */
    ShaderReadOnlyOptimal,

    /** Optimal for shader read */
    DepthStencilReadOnlyOptimal,

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
    std::vector<SRSRenderPassSubpassAttachmentRef> InputAttachmentRefs;

    bool operator==(const SRSRenderPassSubpass& InOther) const
    {
        return ColorAttachmentRefs == InOther.ColorAttachmentRefs &&
            DepthAttachmentRefs == InOther.DepthAttachmentRefs &&
            InputAttachmentRefs == InOther.InputAttachmentRefs;
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
    std::array<CRSTexture*, GMaxRenderTargetPerFramebuffer> ColorRTs;

    /** Depth render targets */
    std::array<CRSTexture*, GMaxRenderTargetPerFramebuffer> DepthRTs;

    SRSFramebuffer() 
    {
        memset(ColorRTs.data(), 0, sizeof(ColorRTs));
        memset(DepthRTs.data(), 0, sizeof(DepthRTs));
    }

	bool operator==(const SRSFramebuffer& InFramebuffer) const
	{
        return ColorRTs == InFramebuffer.ColorRTs && DepthRTs == InFramebuffer.DepthRTs;
	}
};

struct SRSFramebufferHash
{
	std::size_t operator()(const SRSFramebuffer& InFramebuffer) const noexcept
	{
		std::size_t Seed = 0;

		for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
		{
			HashCombine(Seed, InFramebuffer.ColorRTs[i]);
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

    bool operator==(const SVertexInputBindingDescription& InOther) const
    {
        return Binding == InOther.Binding &&
            Stride == InOther.Stride &&
            InputRate == InOther.InputRate;
    }
};

struct SVertexInputBindingDescriptionHash
{
	uint64_t operator()(const SVertexInputBindingDescription& InDesc) const
	{
		uint64_t Hash = 0;

		HashCombine(Hash, InDesc.Binding);
		HashCombine(Hash, InDesc.Stride);
		HashCombine(Hash, InDesc.InputRate);

		return Hash;
	}
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

	bool operator==(const SVertexInputAttributeDescription& InOther) const
	{
		return Binding == InOther.Binding &&
            Location == InOther.Location &&
			Format == InOther.Format &&
			Offset == InOther.Offset;
	}
};

struct SVertexInputAttributeDescriptionHash
{
	uint64_t operator()(const SVertexInputAttributeDescription& InDesc) const
	{
		uint64_t Hash = 0;

		HashCombine(Hash, InDesc.Binding);
		HashCombine(Hash, InDesc.Location);
		HashCombine(Hash, InDesc.Format);
		HashCombine(Hash, InDesc.Offset);

		return Hash;
	}
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
 * Render target blend desc
 */
struct SRSRenderTargetBlendDesc
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
    SRSRenderTargetBlendDesc(
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

    bool operator==(const SRSRenderTargetBlendDesc& InOther) const
    {
        return bEnableBlend == InOther.bEnableBlend &&
            SrcColor == InOther.SrcColor && 
            DstColor == InOther.DstColor && 
            ColorOp == InOther.ColorOp && 
            SrcAlpha == InOther.SrcAlpha && 
            DstAlpha == InOther.DstAlpha && 
            AlphaOp == InOther.AlphaOp;
    }
};

struct SRSRenderTargetBlendDescHash
{
	uint64_t operator()(const SRSRenderTargetBlendDesc& InDesc) const
	{
		uint64_t Hash = 0;

		HashCombine(Hash, InDesc.bEnableBlend);
		HashCombine(Hash, InDesc.SrcColor);
		HashCombine(Hash, InDesc.DstColor);
		HashCombine(Hash, InDesc.ColorOp);
		HashCombine(Hash, InDesc.SrcAlpha);
		HashCombine(Hash, InDesc.DstAlpha);
		HashCombine(Hash, InDesc.AlphaOp);

		return Hash;
	}
};

/**
 * Blend state
 */
struct SRSBlendState
{
    std::vector<SRSRenderTargetBlendDesc> BlendDescs;

    SRSBlendState(const std::vector<SRSRenderTargetBlendDesc>& InBlendDescs 
        = { SRSRenderTargetBlendDesc() })  : BlendDescs(InBlendDescs) {}

    bool operator==(const SRSBlendState& InOther) const
    {
        return BlendDescs == InOther.BlendDescs;
    }
};

struct SRSBlendStateHash
{
	uint64_t operator()(const SRSBlendState& InState) const
	{
		uint64_t Hash = 0;

        for(const auto& Desc : InState.BlendDescs)
            HashCombine<SRSRenderTargetBlendDesc, SRSRenderTargetBlendDescHash>(Hash, Desc);

		return Hash;
	}
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

    bool operator==(const SRSDepthStencilOp& InOther) const
    {
        return FailOp == InOther.FailOp &&
            PassOp == InOther.PassOp &&
            DepthFailOp == InOther.DepthFailOp &&
            CompareOp == InOther.CompareOp;
    }
};

struct SRSDepthStencilOpHash
{
	uint64_t operator()(const SRSDepthStencilOp& InOp) const
	{
		uint64_t Hash = 0;

		HashCombine(Hash, InOp.FailOp);
		HashCombine(Hash, InOp.PassOp);
		HashCombine(Hash, InOp.DepthFailOp);
		HashCombine(Hash, InOp.CompareOp);

		return Hash;
	}
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

    bool operator==(const SRSDepthStencilState& InOther) const
    {
        return bEnableDepthTest == InOther.bEnableDepthTest &&
            bEnableDepthWrite == InOther.bEnableDepthWrite && 
            DepthCompareOp == InOther.DepthCompareOp &&
            bDepthBoundsTestEnable == InOther.bDepthBoundsTestEnable &&
            bStencilTestEnable == InOther.bStencilTestEnable &&
            FrontFace == InOther.FrontFace &&
            BackFace == InOther.BackFace;
    }
};

struct SRSDepthStencilStateHash
{
	uint64_t operator()(const SRSDepthStencilState& InState) const
	{
		uint64_t Hash = 0;

		HashCombine(Hash, InState.bEnableDepthTest);
		HashCombine(Hash, InState.bEnableDepthWrite);
		HashCombine(Hash, InState.DepthCompareOp);
		HashCombine(Hash, InState.bDepthBoundsTestEnable);
		HashCombine(Hash, InState.bStencilTestEnable);
		HashCombine<SRSDepthStencilOp, SRSDepthStencilOpHash>(Hash, InState.FrontFace);
		HashCombine<SRSDepthStencilOp, SRSDepthStencilOpHash>(Hash, InState.BackFace);

		return Hash;
	}
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
        const EFrontFace& InFrontFace = EFrontFace::Clockwise,
        const bool& bInEnableDepthClamp = false,
        const bool& bInEnableRasterizerDiscard = false) :
        PolygonMode(InPolygonMode), CullMode(InCullMode), FrontFace(InFrontFace),
        bEnableDepthClamp(bInEnableDepthClamp), 
        bEnableRasterizerDiscard(bInEnableRasterizerDiscard) {}

    bool operator==(const SRSRasterizerState& InOther) const
    {
        return PolygonMode == InOther.PolygonMode &&
            CullMode == InOther.CullMode &&
            FrontFace == InOther.FrontFace &&
            bEnableDepthClamp == InOther.bEnableDepthClamp &&
            bEnableRasterizerDiscard == InOther.bEnableRasterizerDiscard;
    }
};

struct SRSRasterizerStateHash
{
    uint64_t operator()(const SRSRasterizerState& InState) const
    {
        uint64_t Hash = 0;

        HashCombine(Hash, InState.PolygonMode);
        HashCombine(Hash, InState.CullMode);
        HashCombine(Hash, InState.FrontFace);
        HashCombine(Hash, InState.bEnableDepthClamp);
        HashCombine(Hash, InState.bEnableRasterizerDiscard);

        return Hash;
    }
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

    bool operator==(const SRSPipelineShaderStage& InOther) const
    {
        return Stage == InOther.Stage &&
            Shader == InOther.Shader &&
            EntryPoint == InOther.EntryPoint;
    }
};

struct SRSPipelineShaderStageHash
{
	uint64_t operator()(const SRSPipelineShaderStage& InStage) const
	{
		uint64_t Hash = 0;

		HashCombine(Hash, InStage.Stage);
		HashCombine(Hash, InStage.Shader);
		HashCombine(Hash, InStage.EntryPoint);

		return Hash;
	}
};

/**
 * A pipeline object
 */
class CRSPipeline : public CRSResource
{
public:
    CRSPipeline(
        const SRSResourceCreateInfo& InCreateInfo) : CRSResource(InCreateInfo) {}
};

/**
 * Represents a compute pipeline
 */
struct SRSComputePipeline
{
};

/**
 * Represents a graphics pipeline
 */
struct SRSGraphicsPipeline
{
    std::vector<SRSPipelineShaderStage> ShaderStages;
    std::vector<SVertexInputBindingDescription> BindingDescriptions;
    std::vector<SVertexInputAttributeDescription> AttributeDescriptions;
    SRSBlendState BlendState;
    SRSRasterizerState RasterizerState;
    SRSDepthStencilState DepthStencilState;

    SRSGraphicsPipeline() = default;
    SRSGraphicsPipeline(const std::vector<SRSPipelineShaderStage>& InStages,
        const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
        const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
        const SRSBlendState& InBlendState,
        const SRSRasterizerState& InRasterizerState,
        const SRSDepthStencilState& InDepthStencilState) : ShaderStages(InStages),
        BindingDescriptions(InBindingDescriptions), AttributeDescriptions(InAttributeDescriptions),
        BlendState(InBlendState), RasterizerState(InRasterizerState), 
        DepthStencilState(InDepthStencilState) {}

    bool operator==(const SRSGraphicsPipeline& InOther) const
    {
        return ShaderStages == InOther.ShaderStages &&
            BindingDescriptions == InOther.BindingDescriptions &&
            AttributeDescriptions == InOther.AttributeDescriptions &&
            BlendState == InOther.BlendState &&
            RasterizerState == InOther.RasterizerState &&
            DepthStencilState == InOther.DepthStencilState;
    }
};

struct SRSGraphicsPipelineHash
{
    uint64_t operator()(const SRSGraphicsPipeline& InPipeline) const
    {
        uint64_t Hash = 0;

        for(const auto& Stage : InPipeline.ShaderStages)
            HashCombine<SRSPipelineShaderStage, SRSPipelineShaderStageHash>(Hash, Stage);

		for (const auto& BindingDescription : InPipeline.BindingDescriptions)
			HashCombine<SVertexInputBindingDescription, SVertexInputBindingDescriptionHash>(Hash,
                BindingDescription);

		for (const auto& AttributeDescription : InPipeline.AttributeDescriptions)
			HashCombine<SVertexInputAttributeDescription, SVertexInputAttributeDescriptionHash>(Hash,
				AttributeDescription);

        HashCombine<SRSBlendState, SRSBlendStateHash>(Hash, InPipeline.BlendState);
        HashCombine<SRSRasterizerState, SRSRasterizerStateHash>(Hash, InPipeline.RasterizerState);
        HashCombine<SRSDepthStencilState, SRSDepthStencilStateHash>(Hash, 
            InPipeline.DepthStencilState);

        return Hash;
    }
};

/**
 * A graphics pipeline used for graphics operations
 */
class CRSGraphicsPipeline : public CRSPipeline
{
public:
    CRSGraphicsPipeline(const SRSGraphicsPipeline& InGraphicsPipeline,
        const SRSResourceCreateInfo& InCreateInfo) :
        CRSPipeline(InCreateInfo) {}
};

#pragma endregion


/** Intrusive ptrs */
using CRSSurfacePtr = boost::intrusive_ptr<CRSSurface>;
using CRSBufferPtr = boost::intrusive_ptr<CRSBuffer>;
using CRSTexturePtr = boost::intrusive_ptr<CRSTexture>;
using CRSShaderPtr = boost::intrusive_ptr<CRSShader>;
using CRSGraphicsPipelinePtr = boost::intrusive_ptr<CRSGraphicsPipeline>;

}