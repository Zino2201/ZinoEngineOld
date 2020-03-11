#pragma once

#include "Core/EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Renderer/SceneRenderer.h"
#include "Render/Renderer/Mesh/MeshCollection.h"
#include "Render/Renderer/ProxyShaderParameters.h"
#include "Render/UniformBuffer.h"
#include "Render/Renderer/Mesh/MeshPass.h"

/**
 * Proxies used by the render thread to draw the scene and other
 */

enum class ERenderableComponentProxyMeshUpdateFrequency
{
    Static,
    Dynamic
};

/**
 * Mesh collection of a proxy
 */
class CProxyMeshCollection : public CMeshCollection
{
    friend class CRenderableComponentProxy;

public:
    CProxyMeshCollection(CRenderableComponentProxy* InProxy,
        const SProxyStaticMeshData& InStaticMeshData);
private:
    CRenderableComponentProxy* Proxy;
};

/**
 * Represents a cached command
 */
struct SProxyCachedCommand
{
    EMeshPass Pass;
    SSetElementId Command;
};
    
/**
 * Renderable component proxy base class
 */
class CRenderableComponentProxy
{
    friend class CScene;
    friend class CMeshPass;
    friend class CMeshPassDrawCommand;
    friend class CSceneRenderableProxyDataManager;

public:
    CRenderableComponentProxy(const CRenderableComponent* InComponent);
    virtual ~CRenderableComponentProxy();

    virtual void InitRenderThread();
    virtual ERenderableComponentProxyMeshUpdateFrequency GetMeshUpdateFrequency() const { return ERenderableComponentProxyMeshUpdateFrequency::Static; }
    
    /**
     * Get static mesh data
     */
    virtual std::vector<SProxyStaticMeshData> GetStaticMeshDatas() 
    { 
        return {};
    }

    void CacheDrawCommands();

    /** Update cached command's instance datas */
    void UpdateCachedCommandInstanceDatas();

    IRenderSystemUniformBuffer* GetUniformBuffer() const { return UniformBuffer.GetUniformBuffer(); }

    // TEST!!
    virtual IRenderSystemUniformBuffer* GetLightUBO() const { return nullptr; }

    const STransform& GetTransform() const { return Transform; }
    const SProxyShaderPerInstanceData& GetPerInstanceData() const { return PerInstanceData; }
protected:
    virtual void UpdateUniformBuffer();
protected:
    CScene* Scene;

    STransform Transform;

    /** Per instance data for this proxy, updated when required */
    TUniformBuffer<SProxyShaderPerInstanceData> UniformBuffer;

    /** Proxy meshes of that component */
    std::vector<CProxyMeshCollection> ProxyMeshes;

    /** Indices of cached draw commands generated by this proxy */
    std::vector<SProxyCachedCommand> DrawCommands;

    SProxyShaderPerInstanceData PerInstanceData;

    uint32_t RenderableDataIdx;
    uint32_t RenderableDataOffset;
};