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
    template<typename Data>
    const Data& AddRenderPass(const std::string_view& InName,
        const std::function<void(CRenderPass& InRenderPass, Data& InData)>& InSetupFunc,
        const std::function<void(IRenderSystemContext* InContext, 
            const Data& InData)>& InExecutionFunc)
    {
        RenderPasses.push_back(std::make_unique<CRenderPassLambda<Data>>(
            AvailableRenderPassID++,
            InName,
            *this,
            InSetupFunc, InExecutionFunc));

        return static_cast<CRenderPassLambda<Data>*>(RenderPasses.back().get())->Data;
    }

	SRenderPassResource& CreateResource(ERenderPassResourceType InType);
private:
    bool ValidateRenderPass(const CRenderPass& InRenderPass);
    void SortRenderPasses(std::vector<size_t>& ExecutionLayout);
    bool DependOn(const CRenderPass& InLeft, const CRenderPass& InRight);
    void BuildPhysicalResources(const CRenderPass& InRenderPass);
    void BuildPhysicalRenderPass(CRenderPass& InRenderPass);
private:
    std::vector<std::unique_ptr<CRenderPass>> RenderPasses;
    std::unordered_map<uint32_t, SRenderPassResource> ResourceMap;
    std::unordered_map<uint32_t, CRSTexturePtr> TextureResourceMap;
    uint32_t AvailableRenderPassID;
    uint32_t AvailableResourceID;
    uint32_t AvailablePhysicalResourceID;
    std::vector<size_t> ExecutionLayout;
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