#pragma once

#include "WorldComponent.h"

class IShaderAttributesManager;
class CMaterial;
class CRenderableComponentProxy;

/**
 * A world component that can be rendered
 */
ZCLASS()
class CRenderableComponent : public CWorldComponent
{
    friend class CScene;

public:
    CRenderableComponent();
    virtual ~CRenderableComponent();

    virtual void Initialize() override;
    virtual void Destroy() override;

    /**
     * Instantiate the actual proxy
     */
    virtual CRenderableComponentProxy* InstantiateRenderProxy() const;

    void SetMaterial(const std::shared_ptr<CMaterial> &InMaterial);

    /**
     * Create render proxy
     * Add it to the scene
     */
    void CreateRenderProxy();

    /**
     * Recreate render proxy
     */
    void RecreateRenderProxy();

    /**
     * Delete render proxy
     */
    void DeleteRenderProxy();

    /**
     * Queue a render proxy update
     */
    void NeedRenderProxyUpdate();

    /**
     * Initialize static variables
     */
    static void InitStatics();

    /**
     * Add a component to the render proxy update queue
     */
    static void UpdateRenderProxy(CRenderableComponent* InComponent);

    /**
     * Called at the end of the frame; update render proxies
     */
    static void UpdateRenderProxies();

    const std::shared_ptr<CMaterial>& GetMaterial() const { return Material; }
public:
    static TMulticastDelegate<CRenderableComponent*> OnQueueRenderProxyUpdate;
private:
    static std::vector<CRenderableComponent*> RenderableComponentsToUpdate;
protected:
    std::shared_ptr<CMaterial> Material;
    CRenderableComponentProxy* RenderProxy;
};