#include "Engine/Components/RenderableComponent.h"
#include "Reflection/Builders.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Engine/World.h"
#include "Renderer/WorldProxy.h"
#include "Engine/Components/TransformComponent.h"

namespace ZE::Components
{

void CRenderableComponentSystem::Initialize(ECS::CEntityManager& InEntityManager)
{
	InEntityManager.GetOnComponentAdded().Bind(std::bind(
		&CRenderableComponentSystem::OnComponentAdded, this,
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));
}

void CRenderableComponentSystem::Tick(ECS::CEntityManager& InEntityManager, 
	const float& InDeltaTime)
{
	for(const auto& Component : ComponentsToUpdate)
	{
		if(!Component)
			continue;

		DeleteProxy(InEntityManager, Component);
		CreateProxy(InEntityManager, Component->ParentEntity, Component);
	}

	ComponentsToUpdate.clear();

	auto Trs = InEntityManager.GetComponents(STransformComponent::GetStaticStruct());

	for(const auto& Component : *Trs)
	{
		if(STransformComponent* Transform = Cast<STransformComponent>(Component))
		{
			Transform->Transform.Position.Z += 0.002f;
		}
	}
}

void CRenderableComponentSystem::AddRenderableComponentToUpdate(SRenderableComponent* InComponent)
{
	ComponentsToUpdate.emplace_back(InComponent);
}

void CRenderableComponentSystem::CreateProxy(ECS::CEntityManager& InEntityManager, 
	const ECS::EntityID& InEntityID, 
	SRenderableComponent* InComponent)
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
		TOwnerPtr<Renderer::CRenderableComponentProxy> Proxy = InComponent->InstantiateProxy(
			InEntityManager.GetWorld().GetProxy());
		if(!Proxy)
			return;

		InComponent->Proxy = Proxy;
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

void CRenderableComponentSystem::DeleteProxy(ECS::CEntityManager& InEntityManager, 
	SRenderableComponent* InComponent)
{
	if (!InComponent->Proxy)
		return;

	InEntityManager.GetWorld().GetProxy()->RemoveComponent(InComponent->Proxy);
	InComponent->Proxy = nullptr;
}

void CRenderableComponentSystem::OnComponentAdded(ECS::CEntityManager& InEntityManager, 
	const ECS::EntityID& InEntityID, ECS::SEntityComponent* InComponent)
{
	/** Check if the component added derive from SRenderableComponent */
	if(SRenderableComponent* RenderableComp = Cast<SRenderableComponent>(InComponent))
	{
		CreateProxy(InEntityManager, InEntityID, RenderableComp);
	}
}

void CRenderableComponentSystem::OnComponentRemoved(ECS::CEntityManager& InEntityManager, 
	const ECS::EntityID& InEntityID,
	ECS::SEntityComponent* InComponent)
{
	/** Check if the component removed derive from SRenderableComponent */
	if (SRenderableComponent* RenderableComp = Cast<SRenderableComponent>(InComponent))
	{
		DeleteProxy(InEntityManager, RenderableComp);
	}
}

} /** namespace Components */