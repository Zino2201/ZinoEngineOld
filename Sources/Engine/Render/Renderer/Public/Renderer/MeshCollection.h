#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Renderer/MeshRenderPass.h"
#include <unordered_set>

namespace ZE::Renderer
{

class CProxyDrawCommand;
class CRenderableComponentProxy;

struct SMeshCollectionInstance
{
    TOwnerPtr<CRenderableComponentProxy> Proxy;
    uint32_t IndexCount;
    uint32_t VertexCount;
    EMeshRenderPass RenderPassFlags;

    SMeshCollectionInstance(const TOwnerPtr<CRenderableComponentProxy>& InProxy,
        const uint32_t& InIndexCount, const uint32_t& InVertexCount,
        EMeshRenderPass InRenderPassFlags) : 
        Proxy(InProxy), IndexCount(InIndexCount), VertexCount(InVertexCount),
        RenderPassFlags(InRenderPassFlags) {}
};

/**
 * A collection of mesh instances using the same material and vertex/index buffer
 */
class CMeshCollection
{
    friend class CMeshRenderPass;

public:
    CMeshCollection(CRSGraphicsPipeline* InPipeline,
        CRSBuffer* InVertexBuffer,
        CRSBuffer* InIndexBuffer,
        EIndexFormat InIndexFormat);

    size_t AddInstance(const TOwnerPtr<CRenderableComponentProxy>& InProxy,
        const uint32_t& InIndexCount, const uint32_t& InVertexCount,
        EMeshRenderPass InRenderPassFlags);

    CRSGraphicsPipeline* GetPipeline() const { return Pipeline.get(); }
    CRSBuffer* GetVertexBuffer() const { return VertexBuffer.get(); }
    CRSBuffer* GetIndexBuffer() const { return IndexBuffer.get(); }
    EIndexFormat GetIndexFormat() const { return IndexFormat; }
    SMeshCollectionInstance& GetInstance(const size_t& InIndex) { return Instances[InIndex]; }
    const SMeshCollectionInstance& GetInstance(const size_t& InIndex) const { return Instances[InIndex]; }
    const auto& GetDrawCommands(const EMeshRenderPass& InRenderPass) const { return DrawCommands[InRenderPass]; }
private:
    CRSGraphicsPipelinePtr Pipeline;
    CRSBufferPtr VertexBuffer;
    CRSBufferPtr IndexBuffer;
    EIndexFormat IndexFormat;
    std::vector<SMeshCollectionInstance> Instances;
    mutable std::unordered_map<EMeshRenderPass, std::unordered_set<CProxyDrawCommand*>> DrawCommands;
};

}