#pragma once

#include "Engine/ECS.h"

namespace ZE::Renderer
{
class CRenderableComponentProxy;
class CWorldProxy;
}

namespace ZE::Components
{

/**
 * Base struct for component with render data that can be rendered by the renderer
 */
struct SRenderableComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(SRenderableComponent, SEntityComponent)

    Renderer::CRenderableComponentProxy* Proxy;

    /** Notify the renderable system if the component proxy should be updated */
    bool bHasBeenUpdated;

    /**
     * Utility function for instanciating a proxy
     */
    virtual TOwnerPtr<Renderer::CRenderableComponentProxy> InstantiateProxy(Renderer::CWorldProxy* InWorld) 
        const { return nullptr; }

    SRenderableComponent() : Proxy(nullptr), bHasBeenUpdated(false) {}
};
DECLARE_REFL_TYPE(SRenderableComponent);

/**
 * System for SRenderableComponent
 * This system interact with the renderer to inform it when a new renderable component
 * has been added, or updated
 */

class CRenderableComponentSystem : public ECS::IEntityComponentSystem
{
    DECLARE_REFL_STRUCT_OR_CLASS1(CRenderableComponentSystem, IEntityComponentSystem)

public:
    void Initialize(ECS::CEntityManager& InEntityManager) override;
    void Tick(ECS::CEntityManager& InEntityManager, const float& InDeltaTime) override;
    ZE::Refl::CStruct* GetComponentStruct() const override
    { 
        return ZE::Refl::CStruct::Get<SRenderableComponent>(); 
    }
    bool ShouldTick() const override { return true; }
    uint32_t GetPriority() const override { return 0; }
private:
    void OnComponentAdded(ECS::CEntityManager& InEntityManager, const ECS::EntityID& InEntityID, 
        ECS::SEntityComponent* InComponent);
	void OnComponentRemoved(ECS::CEntityManager& InEntityManager, const ECS::EntityID& InEntityID,
		ECS::SEntityComponent* InComponent);
public:
    TMulticastDelegate<> OnRenderableComponentUpdated;
};
DECLARE_REFL_TYPE(CRenderableComponentSystem);

}