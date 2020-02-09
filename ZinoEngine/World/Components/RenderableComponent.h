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
public:
    CRenderableComponent();
    virtual ~CRenderableComponent();

    /**
     * Render proxy function
     */
    virtual CRenderableComponentProxy* CreateRenderProxy() const;
    void SetMaterial(const std::shared_ptr<CMaterial> &InMaterial);

    const std::shared_ptr<IShaderAttributesManager>& GetShaderAttributesManager() const { return ShaderAttributesManager; }
protected:
    std::shared_ptr<IShaderAttributesManager> ShaderAttributesManager;
    std::shared_ptr<CMaterial> Material;
    std::unique_ptr<CRenderableComponentProxy> RenderProxy;
};