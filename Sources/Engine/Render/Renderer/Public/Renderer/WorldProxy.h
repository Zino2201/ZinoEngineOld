#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include "ProxyDrawCommand.h"
#include "MeshRenderPass.h"
#include <unordered_set>
#include "MeshCollection.h"

namespace ZE::Renderer
{

class CRenderableComponentProxy;
struct SStaticProxyData;

/**
 * Class that manage draw commands for a world proxy
 */
class CProxyDrawCommandManager
{
public:
    CProxyDrawCommand* AddCommand(EMeshRenderPass InRenderPass, 
        const CProxyDrawCommand& InDrawCommand);
    void RemoveCommand(const CProxyDrawCommand& InDrawCommand);

    auto& GetDrawCommands(const EMeshRenderPass& InRenderPass) { return DrawCommands[InRenderPass]; }
private:
    std::unordered_map<EMeshRenderPass, std::vector<CProxyDrawCommand>> DrawCommands;
};

/**
 * Render-thread version of the world
 */
class RENDERER_API CWorldProxy final : public CNonCopyable
{
public:
    CWorldProxy();
    ~CWorldProxy();

    void AddComponent(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
    void RemoveComponent(CRenderableComponentProxy* InProxy);

    CProxyDrawCommandManager& GetDrawCommandManager() { return DrawCommandManager; }
    CMeshCollection& GetCachedMeshCollection(const size_t& InIdx) { return CachedCollections[InIdx]; }
    const auto& GetCachedMeshCollections() const { return CachedCollections; }
private:
    void AddComponent_RenderThread(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
    void RemoveComponent_RenderThread(CRenderableComponentProxy* InProxy);
    bool IsCompatible(const CMeshCollection& InCollection, const SStaticProxyData& InProxyData) const;
private:
    std::vector<std::unique_ptr<CRenderableComponentProxy>> Proxies;
    std::vector<CMeshCollection> CachedCollections;
    CProxyDrawCommandManager DrawCommandManager;
};

}