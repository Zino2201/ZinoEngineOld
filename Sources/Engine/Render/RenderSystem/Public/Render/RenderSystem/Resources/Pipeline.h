#pragma once

#include "Resource.h"
#include "Shader/ShaderCore.h"

using namespace ze::gfx::shaders;

namespace ze
{

namespace gfx::shaders
{
/** Forward */
class CRSShader;
}

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

		hash_combine(Hash, InDesc.Binding);
		hash_combine(Hash, InDesc.Stride);
		hash_combine(Hash, InDesc.InputRate);

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

		hash_combine(Hash, InDesc.Binding);
		hash_combine(Hash, InDesc.Location);
		hash_combine(Hash, InDesc.Format);
		hash_combine(Hash, InDesc.Offset);

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

		hash_combine(Hash, InDesc.bEnableBlend);
		hash_combine(Hash, InDesc.SrcColor);
		hash_combine(Hash, InDesc.DstColor);
		hash_combine(Hash, InDesc.ColorOp);
		hash_combine(Hash, InDesc.SrcAlpha);
		hash_combine(Hash, InDesc.DstAlpha);
		hash_combine(Hash, InDesc.AlphaOp);

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
            hash_combine<SRSRenderTargetBlendDesc, SRSRenderTargetBlendDescHash>(Hash, Desc);

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

		hash_combine(Hash, InOp.FailOp);
		hash_combine(Hash, InOp.PassOp);
		hash_combine(Hash, InOp.DepthFailOp);
		hash_combine(Hash, InOp.CompareOp);

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
        bEnableDepthWrite(bInDepthWriteEnable),
		DepthCompareOp(InComparaisonOp),
		bDepthBoundsTestEnable(bInDepthBoundsTestEnable),
		bStencilTestEnable(bInStencilTestEnable), 
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

		hash_combine(Hash, InState.bEnableDepthTest);
		hash_combine(Hash, InState.bEnableDepthWrite);
		hash_combine(Hash, InState.DepthCompareOp);
		hash_combine(Hash, InState.bDepthBoundsTestEnable);
		hash_combine(Hash, InState.bStencilTestEnable);
		hash_combine<SRSDepthStencilOp, SRSDepthStencilOpHash>(Hash, InState.FrontFace);
		hash_combine<SRSDepthStencilOp, SRSDepthStencilOpHash>(Hash, InState.BackFace);

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

        hash_combine(Hash, InState.PolygonMode);
        hash_combine(Hash, InState.CullMode);
        hash_combine(Hash, InState.FrontFace);
        hash_combine(Hash, InState.bEnableDepthClamp);
        hash_combine(Hash, InState.bEnableRasterizerDiscard);

        return Hash;
    }
};

/**
 * An pipeline shader stage
 */
struct SRSPipelineShaderStage
{
    ShaderStage Stage;
    gfx::shaders::CRSShader* Shader;
    const char* EntryPoint;

    SRSPipelineShaderStage(
		const ShaderStage& InStage,
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

		hash_combine(Hash, InStage.Stage);
		hash_combine(Hash, InStage.Shader);
		hash_combine(Hash, InStage.EntryPoint);

		return Hash;
	}
};

}