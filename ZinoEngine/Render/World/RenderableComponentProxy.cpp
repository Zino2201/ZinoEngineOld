#include "RenderableComponentProxy.h"
#include "World/Components/RenderableComponent.h"
#include "Render/Renderer/Scene.h"

CProxyMeshCollection::CProxyMeshCollection(CRenderableComponentProxy* InProxy,
	const SProxyStaticMeshData& InStaticMeshData) : Proxy(InProxy),
	CMeshCollection(InStaticMeshData)
{
	Instances[0].UniformBuffer = InProxy->GetUniformBuffer();
}

CRenderableComponentProxy::CRenderableComponentProxy(const CRenderableComponent* InComponent) :
	Transform(InComponent->GetTransform()) 
{
	
}

CRenderableComponentProxy::~CRenderableComponentProxy()
{
	UniformBuffer.DestroyResources();
}

void CRenderableComponentProxy::InitRenderThread()
{
	UniformBuffer.InitResources();

	UpdateUniformBuffer();
}

void CRenderableComponentProxy::UpdateUniformBuffer()
{
	PerInstanceData.World = glm::translate(glm::mat4(1.f),
		Transform.Position);
	
	UniformBuffer.Copy(&PerInstanceData);
}

void CRenderableComponentProxy::CacheDrawCommands()
{
	auto Passes = magic_enum::enum_values<EMeshPass>();

	size_t MeshIdx = 0;
	for(const auto& Mesh : ProxyMeshes)
	{
		if (!Mesh.bCanCache)
			continue;
		
		/** Generate a draw command for each pass in pass flags */
		for(const auto& MeshPass : Passes)
		{
			if((Mesh.PassFlags & MeshPass) == MeshPass)
			{
				CMeshPass::GetMeshPass(MeshPass)->BuildDrawCommand(Scene, this, Mesh, MeshIdx);
			}
		}

		MeshIdx++;
	}
}

void CRenderableComponentProxy::UpdateCachedCommandInstanceDatas()
{
	for(const auto& CachedCommand : DrawCommands)
	{
		Scene->GetDrawCommandListManager()
			.GetDrawCommandSet(CachedCommand.Pass)[CachedCommand.Command].UpdateInstancesData();
	}
}