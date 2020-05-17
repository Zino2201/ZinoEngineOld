#include "Renderer/ClusteredForward/ClusteredForwardWorldRenderer.h"
#include "Renderer/FrameGraph/FrameGraph.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystemContext.h"

namespace ZE::Renderer
{

void CClusteredForwardWorldRenderer::Render(CWorldProxy* InWorld, const SWorldRendererView& InView)
{
	must(InView.Surface);
	if(!InView.Surface)
		return;

	/**
	 * Setup the frame graph
	 */
	CFrameGraph Graph;

	struct SBasePass
	{
		SRenderPassResource Color;
		SRenderPassResource Depth;
	};

	const SBasePass& BasePass = Graph.AddRenderPass<SBasePass>("BasePass",
		[&](CRenderPass& InRenderPass, SBasePass& InData)
	{
		SRenderPassTextureInfos DepthInfos;
		DepthInfos.Format = EFormat::D32SfloatS8Uint;
		DepthInfos.Width = InView.Width;
		DepthInfos.Height = InView.Height;

		InData.Color = InRenderPass.CreateRetainedTexture(InView.Surface->GetBackbufferTexture());
		InData.Depth = InRenderPass.CreateTexture(DepthInfos);

		InRenderPass.Write(InData.Color,
			ERSRenderPassAttachmentLayout::Present);
		InRenderPass.Write(InData.Depth,
			ERSRenderPassAttachmentLayout::DepthStencilAttachment);
	},
	[&](IRenderSystemContext* InContext, const SBasePass& InData)
	{
		InContext->SetScissors(
			{
				{
					glm::vec2(0.f, 0.f),
					glm::vec2(InView.Width, InView.Height)
				}
			});
		InContext->SetViewports(
			{
				{
					{
						glm::vec2(0.f, 0.f),
						glm::vec2(InView.Width, InView.Height)
					},
					0.0f, 1.0f
				}
			});

		RenderWorld(InWorld);
	});

	Graph.Compile();
	Graph.Execute();
}

void CClusteredForwardWorldRenderer::RenderWorld(CWorldProxy* InWorld)
{

}

}