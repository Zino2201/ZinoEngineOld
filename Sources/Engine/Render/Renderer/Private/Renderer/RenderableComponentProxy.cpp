#include "Renderer/RenderableComponentProxy.h"

namespace ZE::Renderer
{

CRenderableComponentProxy::CRenderableComponentProxy(CWorldProxy* InWorld, 
	ERenderableComponentProxyCacheMode InCacheMode) : World(InWorld), CacheMode(InCacheMode)
{

}

CRenderableComponentProxy::~CRenderableComponentProxy()
{
	
}

void CRenderableComponentProxy::InitResource_RenderThread()
{
	if(CacheMode == ERenderableComponentProxyCacheMode::Dynamic)
		PerInstanceDataUBO.InitResource();
}

void CRenderableComponentProxy::DestroyResource_RenderThread()
{
	if(CacheMode == ERenderableComponentProxyCacheMode::Dynamic)
		PerInstanceDataUBO.DestroyResource();
}

}