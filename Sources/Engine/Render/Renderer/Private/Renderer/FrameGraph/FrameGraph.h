#pragma once

#include "EngineCore.h"
#include "RenderPass.h"

namespace ZE::Renderer
{

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
    template<typename Data>
	const Data& AddQuadRenderPass(const std::string_view& InName,
		const std::function<void(CRenderPass& InRenderPass, Data& InData)>& InSetupFunc,
		const std::function<void(IRenderSystemContext* InContext, 
            const Data& InData)>& InExecutionFunc,
		const SRSPipelineShaderStage& InVertexStage,
		const SRSPipelineShaderStage& InFragmentStage)
    {
		return AddRenderPass<Data>(InName,
            [=](CRenderPass& InRenderPass, Data& InData)
            {
                InSetupFunc(InRenderPass, InData);
            },
			[=](IRenderSystemContext* InContext, const Data& InData)
            {
                BeginDrawQuadFillingScreen(InContext, InVertexStage, InFragmentStage);
                InExecutionFunc(InContext, InData);
                EndDrawQuadFillingScreen(InContext);
            });
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
    void BeginDrawQuadFillingScreen(IRenderSystemContext* InContext,
        const SRSPipelineShaderStage& InVertexStage,
        const SRSPipelineShaderStage& InFragmentStage);
	void EndDrawQuadFillingScreen(IRenderSystemContext* InContext);
    ERSRenderPassAttachmentLayout TexLayoutToRSLayout(
        const SRenderPassTextureInfos& InInfos, ERenderPassResourceLayout InLayout) const;
private:
    std::vector<std::unique_ptr<CRenderPass>> RenderPasses;
    std::unordered_map<uint32_t, SRenderPassResource> ResourceMap;
    std::unordered_map<uint32_t, CRSTexturePtr> TextureResourceMap;
    uint32_t AvailableRenderPassID;
    uint32_t AvailableResourceID;
    uint32_t AvailablePhysicalResourceID;

    /** Final execution layout of the frame graph */
    std::vector<size_t> ExecutionLayout;

    /** Map used to keep track of texture last layout */
    std::unordered_map<RenderPassResourceID, ERSRenderPassAttachmentLayout> TextureLastLayout;
};

struct SFrameGraphHash
{
    uint64_t operator()(const CFrameGraph& InGraph) const
    {
        uint64_t Hash = 0;

        for(const auto& ExecLayout : InGraph.ExecutionLayout)
        {
            HashCombine(Hash, ExecLayout);
        }

        return Hash;
    }
};

}