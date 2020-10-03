#pragma once

#include "Engine/ECS.h"
#include "Renderer/RenderableComponentInterface.h"
#include "RenderableComponent.gen.h"

namespace ZE::Renderer
{
class CRenderableComponentProxy;
class CWorldProxy;
}

namespace ZE::Components
{

struct STransformComponent;

/**
 * Base struct for component with render data that can be rendered by the renderer
 */
ZSTRUCT()
struct SRenderableComponent : public ECS::SEntityComponent,
    public Renderer::IRenderableComponent
{
    ZE_REFL_BODY()

    Renderer::CRenderableComponentProxy* Proxy;
    STransformComponent* TransformComponent;

    /** Notify the renderable system if the component proxy should be updated */
    ZPROPERTY()
    bool bHasBeenUpdated;

    /**
     * Utility function for instanciating a proxy
     */
    virtual TOwnerPtr<Renderer::CRenderableComponentProxy> InstantiateProxy() const { return nullptr; }

	Math::STransform& GetTransform() const override;

    SRenderableComponent() : Proxy(nullptr), TransformComponent(nullptr), bHasBeenUpdated(false) {}
};
/**
 * System for SRenderableComponent
 * This system interact with the renderer to inform it when a new renderable component
 * has been added, or updated
 */
ZCLASS()
class CRenderableComponentSystem : public ECS::IEntityComponentSystem
{
    ZE_REFL_BODY()

public:
    void Initialize(ECS::CEntityManager& InEntityManager) override;
    void Tick(ECS::CEntityManager& InEntityManager, const float& InDeltaTime) override;
    const ZE::Refl::CStruct* GetComponentStruct() const override
    { 
        return ZE::Refl::GetStruct<SRenderableComponent>(); 
    }
    bool ShouldTick() const override { return true; }
    uint32_t GetPriority() const override { return 0; }
    void AddRenderableComponentToUpdate(SRenderableComponent* InComponent);
    ETickFlagBits GetOrder() const override { return ETickFlagBits::EndOfSimulation; }
private:
    void OnComponentAdded(ECS::CEntityManager& InEntityManager, const ECS::EntityID& InEntityID, 
        ECS::SEntityComponent* InComponent);
	void OnComponentRemoved(ECS::CEntityManager& InEntityManager, const ECS::EntityID& InEntityID,
		ECS::SEntityComponent* InComponent);
    void CreateProxy(ECS::CEntityManager& InEntityManager, 
        const ECS::EntityID& InEntityID, SRenderableComponent* InComponent);
    void DeleteProxy(ECS::CEntityManager& InEntityManager, 
        SRenderableComponent* InComponent);
public:
    TMulticastDelegateNoRet<> OnRenderableComponentUpdated;
private:
    std::vector<SRenderableComponent*> ComponentsToUpdate;
};

}