#include "Renderer/RenderableComponentProxy.h"
#include "Renderer/WorldProxy.h"

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
	must(IsInRenderThread());
	
	PerInstanceDataUBO.InitResource();
	CopyTransformToUBO();
}

void CRenderableComponentProxy::DestroyResource_RenderThread()
{
	must(IsInRenderThread());

	PerInstanceDataUBO.DestroyResource();
}

void CRenderableComponentProxy::CopyTransformToUBO()
{
	glm::mat4 World = glm::mat4(1.0f);

	SRenderableComponentPerInstanceData Data;
	Data.World = World;
	PerInstanceDataUBO.Copy(&Data);
}

void CRenderableComponentProxy::BuildDrawCommands()
{
	must(IsInRenderThread());

	if(CacheMode != ERenderableComponentProxyCacheMode::Cachable)
		return;

	for (const auto& MeshPass : GMeshRenderPasses)
	{
		for (const auto& CollectionInfos : CollectionsIndices)
		{
			auto& MeshCollection = World->GetCachedMeshCollection(
				CollectionInfos.CollectionIndex);

			auto& Instance = MeshCollection.GetInstance(CollectionInfos.InstanceIndex);

			if (HAS_FLAG(Instance.RenderPassFlags, MeshPass))
			{
				CMeshRenderPass::GetMeshPass(MeshPass)->Process(World,
					this, MeshCollection, CollectionInfos.InstanceIndex);
			}
		}
	}
}

}