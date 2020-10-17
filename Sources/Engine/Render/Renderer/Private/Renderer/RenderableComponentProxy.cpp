#include "Renderer/RenderableComponentProxy.h"
#include "Renderer/RenderableComponentInterface.h"

namespace ze::renderer
{

CRenderableComponentProxy::CRenderableComponentProxy(const IRenderableComponent* InComponent,
	const ERenderableComponentProxyType& InType) : Component(InComponent),
	Type(InType)
{
	StaticTransform = InComponent->GetTransform();
	StaticWorldMatrix = StaticTransform.to_world_matrix();
}

}