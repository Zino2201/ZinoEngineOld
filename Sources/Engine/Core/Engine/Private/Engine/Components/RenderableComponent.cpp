#include "Engine/Components/RenderableComponent.h"
#include "Reflection/Builders.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Engine/World.h"
#include "Renderer/WorldProxy.h"
#include "Engine/Components/TransformComponent.h"

namespace ZE
{
namespace Refl
{

REFL_INIT_BUILDERS_FUNC(RenderableComponent)
{
	Builders::TStructBuilder<Components::SRenderableComponent>("SRenderableComponent")
		.Ctor<>();
	Builders::TClassBuilder<Components::CRenderableComponentSystem>("CRenderableComponentSystem")
		.Ctor<>();
}


} /** namespace Refl */

namespace Components
{

void CRenderableComponentSystem::Initialize(ECS::CEntityManager& InEntityManager)
{
	InEntityManager.GetOnComponentAdded().Bind(std::bind(
		&CRenderableComponentSystem::OnComponentAdded, this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));
}

void CRenderableComponentSystem::Tick(ECS::CEntityManager& InEntityManager, const float& InDeltaTime)
{
	/**
	 * Update components proxies that need update
	 */
}

void CRenderableComponentSystem::OnComponentAdded(ECS::CEntityManager& InEntityManager, 
	const ECS::EntityID& InEntityID, ECS::SEntityComponent* InComponent)
{
	/** Check if the component added derive from SRenderableComponent */
	if(SRenderableComponent* RenderableComp = Cast<SRenderableComponent>(InComponent))
	{
		/**
		 * Get parent entity transform
		 */
		if(STransformComponent* Transform = 
			InEntityManager.GetComponent<STransformComponent>(InEntityID))
		{
			/**
			 * Instantiate a proxy for the render thread
			 */
			TOwnerPtr<Renderer::CRenderableComponentProxy> Proxy = RenderableComp->InstantiateProxy(
				InEntityManager.GetWorld().GetProxy());
			if(!Proxy)
				return;

			RenderableComp->Proxy = Proxy;
			Proxy->SetTransform(Transform->Transform);

			InEntityManager.GetWorld().GetProxy()->AddComponent(Proxy);
		}
		else
		{
			must(false);
			LOG(ELogSeverity::Error, Engine, 
				"Error! You must add a Transform Component to entity ID %d to render things.",
				InEntityID);
		}
	}
}

void CRenderableComponentSystem::OnComponentRemoved(ECS::CEntityManager& InEntityManager, 
	const ECS::EntityID& InEntityID,
	ECS::SEntityComponent* InComponent)
{
	/** Check if the component removed derive from SRenderableComponent */
	if (SRenderableComponent* RenderableComp = Cast<SRenderableComponent>(InComponent))
	{
		if(!RenderableComp->Proxy)
			return;

		InEntityManager.GetWorld().GetProxy()->RemoveComponent(RenderableComp->Proxy);
		RenderableComp->Proxy = nullptr;
	}
}

} /** namespace Components */

}