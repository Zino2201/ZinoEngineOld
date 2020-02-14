#include "Scene.h"
#include "World/Components/RenderableComponent.h"
#include "Core/Engine.h"
#include "Render/Commands/RenderCommands.h"

CScene::CScene(CWorld* InWorld) : World(InWorld) {}

void CScene::AddRenderable(CRenderableComponent* InComponent)
{
	/** Create proxy, if nullptr then don't add it */
	CRenderableComponentProxy* Proxy = InComponent->InstantiateRenderProxy();
	InComponent->RenderProxy = Proxy;
	if(!Proxy)
		return;

	/** Add the proxy (should be done in render thread) */
	EnqueueRenderCommand(
		[this, InComponent, Proxy](CRenderCommandList* InCommandList)
		{
			AddRenderable_RenderThread(InComponent, Proxy);
		});
}

void CScene::AddRenderable_RenderThread(CRenderableComponent* InComponent,
	CRenderableComponentProxy* InProxy)
{
	must(IsInRenderThread());

	Proxies.insert(std::make_pair(InComponent, InProxy));
}

void CScene::DeleteRenderable(CRenderableComponent* InComponent)
{
	/** Delete the proxy (should be done in render thread) */
	EnqueueRenderCommand(
		[this, InComponent](CRenderCommandList* InCommandList)
	{
		DeleteRenderable_RenderThread(InComponent);
	});
}

void CScene::DeleteRenderable_RenderThread(CRenderableComponent* InComponent)
{
	must(IsInRenderThread());

	delete Proxies[InComponent];
	Proxies.erase(InComponent);
}