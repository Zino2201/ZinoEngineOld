#include "Renderer/ClusteredForward/ClusteredForwardWorldRenderer.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Renderer/FrameGraph/FrameGraph.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Renderer/WorldProxy.h"
#include "Render/Shader/BasicShader.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Renderer/ProxyDrawCommand.h"
#include "Renderer/PostProcess/BarrelDistortion.h"

namespace ZE::Renderer
{

SRSGraphicsPipeline Test;

CClusteredForwardWorldRenderer::CClusteredForwardWorldRenderer()
{
	std::vector<SVertexInputBindingDescription> BindingDescriptions = 
	{
			SVertexInputBindingDescription(0, 12*2,
				EVertexInputRate::Vertex),
	};

	std::vector<SVertexInputAttributeDescription> AttributeDescriptions = 
	{
		
			SVertexInputAttributeDescription(0, 0, EFormat::R32G32B32Sfloat,
				0),
			SVertexInputAttributeDescription(0, 1, EFormat::R32G32B32Sfloat,
				12),
	};

	std::vector<SRSPipelineShaderStage> Stages = 
	{
		SRSPipelineShaderStage(
			EShaderStage::Vertex,
			CBasicShaderManager::Get().GetShader("BasePassVS")->GetShader(),
			"Main"),
		SRSPipelineShaderStage(
			EShaderStage::Fragment,
			CBasicShaderManager::Get().GetShader("BasePassFS")->GetShader(),
			"Main")
	};

	Test = SRSGraphicsPipeline(
					Stages,
					BindingDescriptions, 
					AttributeDescriptions, 
					SRSBlendState(),
					SRSRasterizerState(
						EPolygonMode::Fill,
						ECullMode::Back,
						EFrontFace::Clockwise),
					SRSDepthStencilState(
						true,
						true,
						ERSComparisonOp::GreaterOrEqual) 
	);
}

void CClusteredForwardWorldRenderer::Render(CWorldProxy* InWorld, const SWorldRendererView& InView)
{
	must(InView.Surface);
	if(!InView.Surface)
		return;

	/**
	 * Setup the frame graph
	 */
	CFrameGraph Graph;

	/** BASE PASS */

	struct SBasePass
	{
		RenderPassResourceID Color;
		RenderPassResourceID Depth;
		RenderPassResourceID Backbuffer;
	};

	const SBasePass& BasePass = Graph.AddRenderPass<SBasePass>("BasePass",
		[&](CRenderPass& InRenderPass, SBasePass& InData)
	{
		SRenderPassTextureInfos ColorInfos;
		ColorInfos.Format = InView.Surface->GetSwapChainFormat();
		ColorInfos.Width = InView.Width;
		ColorInfos.Height = InView.Height;
		ColorInfos.Usage = ERSTextureUsage::RenderTarget | ERSTextureUsage::Sampled;

		SRenderPassTextureInfos DepthInfos;
		DepthInfos.Format = EFormat::D32SfloatS8Uint;
		DepthInfos.Width = InView.Width;
		DepthInfos.Height = InView.Height;

		InData.Color = InRenderPass.CreateTexture(ColorInfos);
		InData.Depth = InRenderPass.CreateTexture(DepthInfos);
		InData.Backbuffer = InRenderPass.CreateRetainedTexture(InView.Surface->GetBackbufferTexture());

		InRenderPass.Write(InData.Color);
		InRenderPass.Write(InData.Depth);
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

		RenderWorld(InContext, InWorld, InView, EMeshRenderPass::BasePass);
	});

	/** POST PROCESS */
	AddBarrelDistortionPass(Graph, BasePass.Color, BasePass.Backbuffer);

	Graph.Compile();
	Graph.Execute();
}

void CClusteredForwardWorldRenderer::RenderWorld(IRenderSystemContext* InContext, 
	CWorldProxy* InWorld,
	const SWorldRendererView& InView,
	EMeshRenderPass InRenderPass)
{
	// TODO: Optimize to reduce v/i buffer binding
	for(const auto& DrawCommand : InWorld->GetDrawCommandManager().GetDrawCommandPool(InRenderPass))
	{
		/** Bind pipeline and v/i buffers */
		InContext->BindGraphicsPipeline(Test);
		InContext->BindVertexBuffers({ DrawCommand.GetVertexBuffer() });
		InContext->BindIndexBuffer({ DrawCommand.GetIndexBuffer() }, 0,
			DrawCommand.GetIndexFormat());

		InContext->SetShaderUniformBuffer(0, 0, InView.ViewDataUBO.get());

		/** Apply draw command shader bindings */
		for (const auto& Binding : DrawCommand.GetBindings())
		{
			InContext->SetShaderUniformBuffer(Binding.Set,
				Binding.Binding,
				Binding.Buffer.get());
		}

		/** Do the actual draw call */
		InContext->DrawIndexed(DrawCommand.GetIndexCount(),
			1,
			0,
			0,
			0);
	}
}

}