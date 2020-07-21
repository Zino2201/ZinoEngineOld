#include "Renderer/WorldRenderer.h"
#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Renderer/RenderableComponentType.h"
#include "Renderer/ClusteredForward/ClusteredForwardRenderingPath.h"
#include "Renderer/MeshRendering/RenderPassDrawcallFactory.h"
#include "Render/RenderSystem/Resources/Texture.h"

namespace ZE::Renderer
{

CWorldRenderer::CWorldRenderer(const SWorldView& InView) : View(InView),
	RenderingPath(std::make_unique<CClusteredForwardRenderingPath>())
{
}

CWorldRenderer::~CWorldRenderer() = default; 

void CWorldRenderer::CheckVisibility()
{
	// TODO: Actual visibility checks...
	for (size_t i = 0; i < View.WorldProxy.GetProxies().size(); ++i)
	{
		VisibleProxies.emplace_back(View.WorldProxy.GetProxies()[i].get());
	}
}

void CWorldRenderer::CopyGameState(TransientPerFrameDataMap& InPerFrameMap)
{
	/** Reserve transient data */
	PerViewData.reserve(VisibleProxies.size());

	// TODO: Sort view data

	for(size_t i = 0; i < VisibleProxies.size(); ++i)
	{
		CRenderableComponentProxy* Proxy = VisibleProxies[i];
		must(Proxy);
		must(Proxy->GetComponentType());

		/** Check if we have frame data in cache, if not, cache it */
		if(!InPerFrameMap.contains(Proxy))
		{
			auto& PerFrameData = InPerFrameMap[Proxy];
			PerFrameData.Proxy = Proxy;
			Proxy->GetComponentType()->CopyPerFrameComponentState(*Proxy, PerFrameData);
		}

		/** Init per view data */
		PerViewData.emplace_back(InPerFrameMap[Proxy]);
		
		// TODO: Move to prepare
		PerViewData[i].World = Proxy->GetStaticWorldMatrix();

		Proxy->GetComponentType()->CopyComponentStatePerView(*Proxy, PerViewData[i]);
	}
}

void CWorldRenderer::Prepare()
{
	/** Update view ubo */
	SWorldView::SWorldViewUBO UBO;
	UBO.ViewProj = View.ViewProj;
	UBO.ViewPos = Math::SVector3Float(View.ViewPos.X, View.ViewPos.Y, View.ViewPos.Z);
	UBO.ViewForward = Math::SVector3Float(View.ViewForward.X, View.ViewForward.Y, View.ViewForward.Z);
	View.TEST_ViewUBO.Copy(UBO);

	/** Gather dynamic meshes from visible proxies */
	size_t Idx = 0;
	for(auto& Proxy : VisibleProxies)
	{
		// TEST
		Proxy->TEST_PerInstanceUBO.Copy(PerViewData[Idx].World);

		/**
		 * Add the proxy dynamic meshes to the world view
		 */
		if(Proxy->GetType() == ERenderableComponentProxyType::Dynamic)
		{
			std::vector<SMesh> Meshes = Proxy->GetDynamicMeshes(View);

			for(const auto& Mesh : Meshes)
			{
				View.VisibleDynamicMeshes.emplace_back(std::move(Mesh));
				// TEST!
				View.VisibleDynamicMeshRenderPassFlags.emplace_back(ERenderPass::BasePass); 
				View.VisibleDynamicMeshProxies.emplace_back(Proxy);
			}
		}

		Idx++;
	}

	/**
	 * Prepare drawcalls for each pass
	 */
	for(const auto [RenderPass, Creator] : RenderPassDrawcallFactory::GetCreators())
	{
		TOwnerPtr<CRenderPassDrawcallFactory> Factory = 
			Creator->Instantiate(View.WorldProxy);

		View.RenderPassRendererMap.insert({ RenderPass, CRenderPassRenderer(View.WorldProxy) });
		View.RenderPassRendererMap.at(RenderPass).PrepareDrawcalls(View,
			RenderPass,
			*Factory);

		Factories.emplace_back(Factory);
	}
}

void CWorldRenderer::Draw()
{
	RenderingPath->Draw(View);
}

}