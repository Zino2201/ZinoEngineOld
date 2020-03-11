#include "Scene.h"
#include "World/Components/RenderableComponent.h"
#include "Core/Engine.h"
#include "Render/World/RenderableComponentProxy.h"

CScene::CScene(CWorld* InWorld) : World(InWorld), 
	RenderableProxyDataManager(std::make_unique<CSceneRenderableProxyDataManager>()) 
{
	g_Engine->GetRenderSystem()->GetFrameCompletedDelegate().Bind(
		std::bind(&CScene::OnFrameCompleted, this));
}

void CScene::AddRenderable(CRenderableComponent* InComponent)
{
	/** Create proxy, if nullptr then don't add it */
	CRenderableComponentProxy* Proxy = InComponent->InstantiateRenderProxy();
	if(!Proxy)
		return;
	InComponent->RenderProxy = Proxy;
	Proxy->Scene = this;

	/** Create a render data */

	/** Add the proxy (should be done in render thread) */
	EnqueueRenderCommand(
		[this, InComponent](CRenderCommandList* InCommandList)
	{
		AddRenderable_RenderThread(InComponent);
	});
}

void CScene::AddRenderable_RenderThread(CRenderableComponent* InComponent)
{
	must(IsInRenderThread());

	CRenderableComponentProxy* Proxy = InComponent->RenderProxy;
	if (!Proxy)
		return;

	Proxy->InitRenderThread();

	Proxies.push_back(Proxy);

	/** Generate drawcalls for that proxy */
	if(Proxy->GetMeshUpdateFrequency() == ERenderableComponentProxyMeshUpdateFrequency::Static)
	{
		std::vector<SProxyStaticMeshData> MeshDatas = Proxy->GetStaticMeshDatas();
		if(MeshDatas.empty())
			return;

		for(const auto& MeshData : MeshDatas)
		{
			/** Create a proxy mesh collection */
			CProxyMeshCollection ProxyMeshCollection(Proxy,
				MeshData);

			Proxy->ProxyMeshes.push_back(ProxyMeshCollection);
		}

		/** Cache draw commands if we can, else they will be built each frame */
		Proxy->CacheDrawCommands();
	}

	RenderableProxyDataManager->EnqueueUpdateProxyData(Proxy);
}

void CScene::DeleteRenderable(CRenderableComponent* InComponent)
{
	CRenderableComponentProxy* Proxy = InComponent->RenderProxy;
	if (!Proxy)
		return;

	InComponent->RenderProxy = nullptr;

	/** Delete the proxy (should be done in render thread) */
	EnqueueRenderCommand(
		[this, Proxy](CRenderCommandList* InCommandList)
	{
		DeleteProxy_RenderThread(Proxy);
	});
}

void CScene::DeleteProxy_RenderThread(CRenderableComponentProxy* InProxy)
{
	must(IsInRenderThread());

	/** Delete all occurrence of the proxy in collections */
	for(CMeshCollection& Collection : MeshCollections)
	{
		Collection.RemoveInstance(InProxy);
	}

	/** Remove cached commands of this proxy */

	/** Remove proxy from proxies list */
	Proxies.erase(std::remove(Proxies.begin(), Proxies.end(), InProxy), Proxies.end());

	/** Free the proxy */
	delete InProxy;
	
	/** Recreate collections using instancing */
}

void CScene::BuildDrawCommands(CRenderableComponentProxy* InProxy,
	const CMeshCollection& InCollection)
{
}

void CScene::OnFrameCompleted()
{
	RenderableProxyDataManager->UpdateProxies();
}

SSetElementId CSceneProxyDrawCommandListManager::AddCommand(EMeshPass InPass, 
	const CMeshPassDrawCommand& InCommand)
{
	/** If we have a command with the same hash, merge them */
	if(Commands[InPass].Contains(InCommand))
	{
		/** Merge them */
		SSetElement<CMeshPassDrawCommand>& Command 
			= Commands[InPass].GetElementByValue(InCommand);

		Command.Element.MergeWith(InCommand);

		return Command.Index;
	}

	return Commands[InPass].Add(InCommand);
}

void CSceneProxyDrawCommandListManager::ExecuteCommands(IRenderCommandContext* InContext, 
	EMeshPass InMeshPass)
{
	if(Commands[InMeshPass].IsEmpty())
		return;

	for(uint64_t i = 0; i < Commands[InMeshPass].GetCount(); ++i)
	{
		auto& Cmd = Commands[InMeshPass][i];

		Commands[InMeshPass][i].Execute(InContext);
	}
}

CSceneRenderableProxyDataManager::CSceneRenderableProxyDataManager() : CurrentSize(0) 
{
	ProxiesData = nullptr;

	ResizeStorageBuffer(g_InstanceCountForResize * sizeof(SProxyShaderPerInstanceData));
}

void CSceneRenderableProxyDataManager::EnqueueUpdateProxyData(CRenderableComponentProxy* InProxyToUpdate)
{
	must(IsInRenderThread());

	ProxiesToUpdate.push_back(InProxyToUpdate);
}

void CSceneRenderableProxyDataManager::UpdateProxies()
{
	must(IsInRenderThread());

	for(const auto& Proxy : ProxiesToUpdate)
	{
		if(!Proxies.Contains(Proxy))
		{
			/** Add the proxy */
			AddProxy(Proxy);
		}
			
		uint8_t* Dst = reinterpret_cast<uint8_t*>(ProxiesData->GetMappedMemory());
		memcpy(Dst + Proxy->RenderableDataOffset, &Proxy->GetPerInstanceData(),
			sizeof(SProxyShaderPerInstanceData));

		Proxy->UpdateCachedCommandInstanceDatas();
	}

	ProxiesToUpdate.clear();
}

void CSceneRenderableProxyDataManager::AddProxy(CRenderableComponentProxy* InProxy)
{
	if (AvailableOffsets.IsEmpty())
		ResizeStorageBuffer(CurrentSize +
			(g_InstanceCountForResize * sizeof(SProxyShaderPerInstanceData)));

	uint32_t Offset = AvailableOffsets.GetElementByIndex(0).Element;

	AvailableOffsets.Remove(Offset);
	UsedOffsets.Add(Offset);

	InProxy->RenderableDataOffset = Offset;
	InProxy->RenderableDataIdx =
		Offset * (CurrentSize / sizeof(SProxyShaderPerInstanceData)) / CurrentSize;

	Proxies.Add(InProxy);
}

void CSceneRenderableProxyDataManager::ResizeStorageBuffer(const uint64_t& InNewSize)
{
	std::vector<void*> OldData;

	/** Copy the data to a temporary buffer if needed */
	if(CurrentSize > 0 && ProxiesData)
	{
		OldData.resize(CurrentSize, nullptr);
		memcpy(OldData.data(), ProxiesData->GetMappedMemory(), CurrentSize);
	}

	/** Recreate storage buffer */
	if(ProxiesData) ProxiesData->Destroy();
	ProxiesData = g_Engine->GetRenderSystem()->CreateBuffer(SRenderSystemBufferInfos(
		InNewSize,
		EBufferUsage::StorageBuffer, 
		EBufferMemoryUsage::CpuToGpu, 
		true, 
		"SceneStorageBuffer"));
	if(!ProxiesData) 
		LOG(ELogSeverity::Fatal, "Failed to create storage buffer");

	/** Copy first old data if we have it */
	if(!OldData.empty())
	{
 		memcpy(ProxiesData->GetMappedMemory(), OldData.data(), CurrentSize);
	}

	CurrentSize = InNewSize;

	/** Fill the available offsets list */
	AvailableOffsets.Empty();

	/** For each offset, check if it is not in the offset map */
	for(uint32_t Offset = 0; Offset < CurrentSize; Offset += sizeof(SProxyShaderPerInstanceData))
	{
		if (!UsedOffsets.Contains(Offset))
		{
			AvailableOffsets.Add(Offset);
		}
	}
}