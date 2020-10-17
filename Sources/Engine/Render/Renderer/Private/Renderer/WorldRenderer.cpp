#include "Renderer/WorldRenderer.h"
#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Renderer/RenderableComponentType.h"
#include "Renderer/ClusteredForward/ClusteredForwardRenderingPath.h"
#include "Renderer/MeshRendering/RenderPassDrawcallFactory.h"
#include "Render/RenderSystem/Resources/Texture.h"

namespace ze::renderer
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
		ZE_CHECK(Proxy);
		ZE_CHECK(Proxy->GetComponentType());

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
	UBO.ViewPos = maths::Vector3f(View.ViewPos.x, View.ViewPos.y, View.ViewPos.z);
	UBO.ViewForward = maths::Vector3f(View.ViewForward.x, View.ViewForward.y, View.ViewForward.z);
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
				View.VisibleDynamicMeshRenderPassFlags.emplace_back(ERenderPassFlagBits::BasePass); 
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
		OwnerPtr<CRenderPassDrawcallFactory> Factory = 
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