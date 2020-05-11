#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Render/RenderThread.h"

namespace ZE::Renderer
{

CWorldProxy::CWorldProxy() = default;
CWorldProxy::~CWorldProxy() = default;

void CWorldProxy::AddComponent(const TOwnerPtr<CRenderableComponentProxy>& InProxy)
{
	EnqueueRenderCommand("CWorldProxy::AddComponent",
		[this, InProxy]()
	{
		AddComponent_RenderThread(InProxy);
	});
}

void CWorldProxy::AddComponent_RenderThread(const TOwnerPtr<CRenderableComponentProxy>& InProxy)
{
	must(IsInRenderThread());

	InProxy->InitResource();
	Proxies.emplace_back(InProxy);
}

void CWorldProxy::RemoveComponent(CRenderableComponentProxy* InProxy)
{
	EnqueueRenderCommand("CWorldProxy::RemoveComponent", 
		[this, InProxy]()
		{
			RemoveComponent_RenderThread(InProxy);
		});
}

void CWorldProxy::RemoveComponent_RenderThread(CRenderableComponentProxy* InProxy)
{
	must(IsInRenderThread());

	/** Remove proxy from vector (this will free the proxy memory) **/
	InProxy->DestroyResource();
	//Proxies.erase(std::remove(Proxies.begin(), Proxies.end(), InProxy), Proxies.end());
}

}