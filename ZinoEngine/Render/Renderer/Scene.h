#pragma once

#include "Render/RenderCore.h"
#include "Render/Renderer/Mesh/MeshCollection.h"
#include "Render/Renderer/Mesh/MeshPass.h"

class CWorld;
class CRenderableComponent;
class IRenderCommandContext;

/**
 * Manager for draw command lists
 */
class CSceneProxyDrawCommandListManager
{
public:
    SSetElementId AddCommand(EMeshPass InPass, const CMeshPassDrawCommand& InCommand);

    TSet<CMeshPassDrawCommand>& GetDrawCommandSet(EMeshPass InPass)
    {
        return Commands[InPass];
    }

    void ExecuteCommands(IRenderCommandContext* InContext, EMeshPass InMeshPass);
private:
    std::map<EMeshPass, TSet<CMeshPassDrawCommand>> Commands;
};

constexpr int g_InstanceCountForResize = 100;

/**
 * Class for managing instancing related data
 */
class CSceneRenderableProxyDataManager
{
public:
    CSceneRenderableProxyDataManager();

    /**
     * Enqueue the proxy update for next frame
     */
    void EnqueueUpdateProxyData(CRenderableComponentProxy* InProxyToUpdate);

    void UpdateProxies();

    CRenderSystemBuffer* GetBuffer() const { return ProxiesData.get(); }
private:
    void AddProxy(CRenderableComponentProxy* InProxy);
    void ResizeStorageBuffer(const uint64_t& InNewSize);
private:
    /**
     * Proxies to update ubo data
     */
    std::vector<CRenderableComponentProxy*> ProxiesToUpdate;

    /** 
     * The proxy data storage buffer 
     * Store all the per instance data of each proxy
     */
    CRenderSystemBufferPtr ProxiesData;

    uint64_t CurrentSize;

    TSet<CRenderableComponentProxy*> Proxies;

    /** Available offsets */
    TSet<uint32_t> AvailableOffsets;

    /** Used offsets */
    TSet<uint32_t> UsedOffsets;
};

/**
 * Represents a world
 */
class CScene
{
public:
    CScene(CWorld* InWorld);

    void AddRenderable(CRenderableComponent* InComponent);
    void DeleteRenderable(CRenderableComponent* InComponent);

    const std::vector<CRenderableComponentProxy*>& GetProxies() const { return Proxies; }
    const std::vector<CMeshCollection>& GetMeshCollections() const { return MeshCollections; }
    CSceneProxyDrawCommandListManager& GetDrawCommandListManager() { return DrawCommandListManager; }
    CSceneRenderableProxyDataManager* GetRenderableProxyDataManager() 
    { 
        return RenderableProxyDataManager.get(); 
    }
private:
    void AddRenderable_RenderThread(CRenderableComponent* InComponent);
	void DeleteProxy_RenderThread(CRenderableComponentProxy* InProxy);

    /**
     * Build draw commands for a proxy
     */
    void BuildDrawCommands(CRenderableComponentProxy* InProxy,
        const CMeshCollection& InCollection);

    void OnFrameCompleted();
private:
    CWorld* World;

    /** All mesh proxies of the scene */
    std::vector<CRenderableComponentProxy*> Proxies;

    /** All mesh collections contained in the scene */
    std::vector<CMeshCollection> MeshCollections;

    /** All draw commands cached */
    std::vector<CMeshPassDrawCommand> DrawCommands;

    /** Draw commands manager */
	CSceneProxyDrawCommandListManager DrawCommandListManager;

    /** Renderable proxy data manger */
	std::unique_ptr<CSceneRenderableProxyDataManager> RenderableProxyDataManager;
};