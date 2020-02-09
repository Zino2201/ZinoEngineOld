#include "RenderThread.h"
#include "World/Components/RenderableComponent.h"

CRenderableComponentProxy::CRenderableComponentProxy(const CRenderableComponent* InComponent) :
	Transform(InComponent->GetTransform()), bIsOutdated(false) {}

void CRenderableComponentProxy::Draw(CRenderCommandList* InCommandList) {}