#include "RenderableComponentProxy.h"
#include "World/Components/RenderableComponent.h"

CRenderableComponentProxy::CRenderableComponentProxy(const CRenderableComponent* InComponent) :
	Transform(InComponent->GetTransform()) {}

void CRenderableComponentProxy::Draw(IRenderCommandContext* InCommandList) {}