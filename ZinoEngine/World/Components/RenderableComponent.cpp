#include "RenderableComponent.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/Material/Material.h"
#include "Core/Engine.h"
#include "Core/RenderThread.h"
#include "World/World.h"
#include "Render/Renderer/Scene.h"
#include "Render/World/RenderableComponentProxy.h"

TMulticastDelegate<CRenderableComponent*> CRenderableComponent::OnQueueRenderProxyUpdate;
std::vector<CRenderableComponent*> CRenderableComponent::RenderableComponentsToUpdate;

CRenderableComponent::CRenderableComponent() {}
CRenderableComponent::~CRenderableComponent() {}

void CRenderableComponent::Initialize()
{
	NeedRenderProxyUpdate();
}

void CRenderableComponent::Destroy()
{
	DeleteRenderProxy();
}

void CRenderableComponent::SetMaterial(const std::shared_ptr<CMaterial>& InMaterial)
{
	Material = InMaterial;

	NeedRenderProxyUpdate();
}

void CRenderableComponent::CreateRenderProxy()
{
	g_Engine->GetWorld()->GetScene()->AddRenderable(this);
}

void CRenderableComponent::RecreateRenderProxy()
{
	DeleteRenderProxy();
	CreateRenderProxy();
}

void CRenderableComponent::DeleteRenderProxy()
{
	g_Engine->GetWorld()->GetScene()->DeleteRenderable(this);
}

void CRenderableComponent::NeedRenderProxyUpdate()
{
	OnQueueRenderProxyUpdate.Broadcast(this);
}

CRenderableComponentProxy* CRenderableComponent::InstantiateRenderProxy() const
{
	return new CRenderableComponentProxy(this);
}

void CRenderableComponent::InitStatics()
{
	OnQueueRenderProxyUpdate.Bind(&CRenderableComponent::UpdateRenderProxy);
}

void CRenderableComponent::UpdateRenderProxy(CRenderableComponent* InComponent)
{
	if(!std::count(RenderableComponentsToUpdate.begin(), RenderableComponentsToUpdate.end(),
		InComponent))
	RenderableComponentsToUpdate.push_back(InComponent);
}

void CRenderableComponent::UpdateRenderProxies()
{
	for (auto RenderableComponent : RenderableComponentsToUpdate)
	{
		RenderableComponent->RecreateRenderProxy();
	}

	RenderableComponentsToUpdate.clear();
}