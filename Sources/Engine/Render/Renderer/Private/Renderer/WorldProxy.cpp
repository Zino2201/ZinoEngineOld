#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Render/RenderThread.h"

namespace ZE::Renderer
{

/** DRAW COMMAND MGR */

CProxyDrawCommand* CProxyDrawCommandManager::AddCommand(EMeshRenderPass InRenderPass, 
	const CProxyDrawCommand& InDrawCommand)
{
	// TODO: Merge draw commands
	auto& Vector = DrawCommands[InRenderPass];

	Vector.push_back(InDrawCommand);
	return &Vector.back();
}

void CProxyDrawCommandManager::RemoveCommand(const CProxyDrawCommand& InDrawCommand)
{
	for(auto& [Pass, Vec] : DrawCommands)
		Vec.erase(std::remove(Vec.begin(), Vec.end(), InDrawCommand));
}

/** WORLD PROXY */

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

	/**
	 * If proxy is cachable, build mesh collections
	 */
	if(InProxy->GetCacheMode() == ERenderableComponentProxyCacheMode::Cachable)
	{
		auto Datas = InProxy->GetStaticProxyData();
		
		for(const auto& Data : Datas)
		{
			bool bHasFoundCollection = false;
			size_t Idx = 0;
			for(auto& Collection : CachedCollections)
			{
				if(IsCompatible(Collection, Data))
				{
					size_t InstanceIdx = Collection.AddInstance(InProxy, Data.IndexCount, 0,
						Data.RenderPassFlags);
					InProxy->CollectionsIndices.emplace_back(Idx, InstanceIdx);
					bHasFoundCollection = true;
					break;
				}

				Idx++;
			}

			/** Create a new mesh collection */
			if(!bHasFoundCollection)
			{
				CachedCollections.emplace_back(nullptr,
					Data.VertexBuffer.get(),
					Data.IndexBuffer.get(),
					Data.IndexFormat);
				size_t InstanceIdx = CachedCollections.back().AddInstance(InProxy, Data.IndexCount,
					0, Data.RenderPassFlags);
				InProxy->CollectionsIndices.emplace_back(CachedCollections.size() - 1,
					InstanceIdx);
			}
		}

		InProxy->BuildDrawCommands();
	}
}

bool CWorldProxy::IsCompatible(const CMeshCollection& InCollection, 
	const SStaticProxyData& InProxyData) const
{
	if(InCollection.GetVertexBuffer() == InProxyData.VertexBuffer &&
		InCollection.GetIndexBuffer() == InProxyData.IndexBuffer)
		return true;

	return false;
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

	LOG(ELogSeverity::Info, None, "DESTROY");

	/** Remove proxy from vector (this will free the proxy memory) **/
	InProxy->DestroyResource();
	{
		size_t Idx = 0;
		for (const auto& Proxy : Proxies)
		{
			if(Proxy.get() == InProxy)
			{
				Proxies.erase(Proxies.begin() + Idx);
				break;
			}

			Idx++;
		}
	}
}

}