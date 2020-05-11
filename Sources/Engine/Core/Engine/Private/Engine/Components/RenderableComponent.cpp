#include "Engine/Components/RenderableComponent.h"
#include "Reflection/Builders.h"
#include "Renderer/RenderableComponentProxy.h"

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
		 * Instantiate a proxy for the render thread
		 */
		TOwnerPtr<Renderer::CRenderableComponentProxy> Proxy = RenderableComp->InstantiateProxy();
		if(!Proxy)
			return;

		RenderableComp->Proxy = Proxy;

	}
}


} /** namespace Components */

}