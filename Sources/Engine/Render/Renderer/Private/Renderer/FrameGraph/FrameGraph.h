#pragma once

#include "EngineCore.h"
#include "RenderPass.h"
#include "Renderer/RendererModule.h"
#include <robin_hood.h>
#include "Render/RenderSystem/RenderSystemContext.h"

namespace ze::renderer
{

/**
 * Quad render pass
 */
template<typename T, typename SetupFunc, typename ExecFunc>
class CRenderPassQuad final : public CRenderPass
{
    friend class CFrameGraph;

public:
    CRenderPassQuad(const uint32_t& InID,
        const std::string_view& InName,
        CFrameGraph& InFrameGraph,
		const SetupFunc& InSetupFunc,
		const ExecFunc& InExecutionFunc,
		const SRSPipelineShaderStage& InVertexStage,
		const SRSPipelineShaderStage& InFragmentStage) : 
        CRenderPass(InID, InName, InFrameGraph),
        Setup(InSetupFunc), Exec(InExecutionFunc), FrameGraph(InFrameGraph),
        VertexStage(InVertexStage), FragmentStage(InFragmentStage)
    {
        Setup(*this, Data);
    }

    void Execute(IRenderSystemContext* InContext) override
    {
		InContext->BindGraphicsPipeline(
			SRSGraphicsPipeline(
				{ VertexStage, FragmentStage },
				{ SVertexInputBindingDescription(0,
					sizeof(SQuadVertex), EVertexInputRate::Vertex) },
			{
				SVertexInputAttributeDescription(0, 0, EFormat::R32G32Sfloat,
					offsetof(SQuadVertex, Position)),
				SVertexInputAttributeDescription(0, 1, EFormat::R32G32Sfloat,
					offsetof(SQuadVertex, TexCoord)),
			},
			SRSBlendState(),
			SRSRasterizerState(
				EPolygonMode::Fill,
				ECullMode::None,
				EFrontFace::Clockwise),
				SRSDepthStencilState()));
        Exec(InContext, Data);
		InContext->BindVertexBuffers({ CRendererModule::QuadVBuffer.get() });
		InContext->BindIndexBuffer(CRendererModule::QuadIBuffer.get(), 0,
			EIndexFormat::Uint16);
		InContext->DrawIndexed(6, 1, 0, 0, 0);
    }
private:
    T Data;
    SetupFunc Setup;
    ExecFunc Exec;
    CFrameGraph& FrameGraph;
    SRSPipelineShaderStage VertexStage;
    SRSPipelineShaderStage FragmentStage;
};

/**
 * Represents a frame graph, that consists of CRenderPass-es
 */
class CFrameGraph final
{
    friend class CRenderPass;
    friend class CFrameGraphManager;
    friend struct SFrameGraphHash;

public:
    CFrameGraph();
    
    /**
     * Compile the frame graph
     */
    bool Compile();

    /**
     * Execute the frame graph
     */
    void Execute();

    /**
     * Add a render pass using lambdas
     * Store Data to be used later at the execution stage
     */
    template<typename Data, typename Setup, typename Exec>
    const Data& AddRenderPass(const std::string_view& InName,
        const Setup& InSetupFunc,
        const Exec& InExecutionFunc)
    {
        RenderPasses.push_back(std::make_unique<CRenderPassLambda<Data, Setup, Exec>>(
            AvailableRenderPassID++,
            InName,
            *this,
            InSetupFunc, InExecutionFunc));

        return static_cast<CRenderPassLambda<Data, Setup, Exec>*>(RenderPasses.back().get())->Data;
    }

    /**
     * Helper function, add a render pass with a quad filling the screen
     * Useful for post process passes
     */
    template<typename Data, typename Setup, typename Exec>
	const Data& AddQuadRenderPass(const std::string_view& InName,
		const Setup& InSetupFunc,
		const Exec& InExecutionFunc,
		const SRSPipelineShaderStage& InVertexStage,
		const SRSPipelineShaderStage& InFragmentStage)
    {
		RenderPasses.push_back(std::make_unique<CRenderPassQuad<Data, Setup, Exec>>(
			AvailableRenderPassID++,
			InName,
			*this,
			InSetupFunc, InExecutionFunc, InVertexStage, InFragmentStage));

		return static_cast<CRenderPassQuad<Data, Setup, Exec>*>(RenderPasses.back().get())->Data;
    }

	SRenderPassResource& CreateResource(ERenderPassResourceType InType);
    CRSTexture* GetTexture(const RenderPassResourceID& InID) 
    { 
        return TextureResourceMap[InID].get();
    }
private:
    bool ValidateRenderPass(const CRenderPass& InRenderPass);
    void SortRenderPasses(std::vector<size_t>& ExecutionLayout);
    bool DependOn(const CRenderPass& InLeft, const CRenderPass& InRight);
    void BuildPhysicalResources(const CRenderPass& InRenderPass);
    void BuildPhysicalRenderPass(CRenderPass& InRenderPass);
    ERSRenderPassAttachmentLayout TexLayoutToRSLayout(
        const SRenderPassTextureInfos& InInfos, ERenderPassResourceLayout InLayout) const;
private:
    std::vector<std::unique_ptr<CRenderPass>> RenderPasses;
    robin_hood::unordered_map<uint32_t, SRenderPassResource> ResourceMap;
    robin_hood::unordered_map<uint32_t, CRSTexturePtr> TextureResourceMap;
    uint32_t AvailableRenderPassID;
    uint32_t AvailableResourceID;
    uint32_t AvailablePhysicalResourceID;

    /** Final execution layout of the frame graph */
    std::vector<size_t> ExecutionLayout;

    /** Map used to keep track of texture last layout */
    robin_hood::unordered_map<RenderPassResourceID, ERSRenderPassAttachmentLayout> TextureLastLayout;
};

struct SFrameGraphHash
{
    uint64_t operator()(const CFrameGraph& InGraph) const
    {
        uint64_t Hash = 0;

        for(const auto& ExecLayout : InGraph.ExecutionLayout)
        {
            hash_combine(Hash, ExecLayout);
        }

        return Hash;
    }
};

}