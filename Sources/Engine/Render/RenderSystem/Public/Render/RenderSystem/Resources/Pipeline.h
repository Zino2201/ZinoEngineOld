#pragma once

#include "Resource.h"
#include "Shader/ShaderCore.h"

namespace ZE
{

/** Forward */
class CRSShader;

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

}