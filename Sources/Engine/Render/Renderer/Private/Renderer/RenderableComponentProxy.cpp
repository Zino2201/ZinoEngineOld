#include "Renderer/RenderableComponentProxy.h"
#include "Renderer/RenderableComponentInterface.h"

namespace ZE::Renderer
{

CRenderableComponentProxy::CRenderableComponentProxy(const IRenderableComponent* InComponent,
	const ERenderableComponentProxyType& InType) : Component(InComponent),
	Type(InType)
{
	StaticTransform = InComponent->GetTransform();
	StaticWorldMatrix = StaticTransform.ToWorldMatrix();
}

}