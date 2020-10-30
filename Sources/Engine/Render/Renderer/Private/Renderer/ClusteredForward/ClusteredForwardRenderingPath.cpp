#include "Renderer/ClusteredForward/ClusteredForwardRenderingPath.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Renderer/FrameGraph/FrameGraph.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Renderer/WorldProxy.h"
#include "Render/Shader/BasicShader.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Renderer/PostProcess/BarrelDistortion.h"
#include "Renderer/RendererModule.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ze::renderer
{

SRSGraphicsPipeline Test;

CClusteredForwardRenderingPath::CClusteredForwardRenderingPath()
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
			ShaderStage::Vertex,
			CBasicShaderManager::Get().GetShader("BasePassVS")->GetShader(),
			"Main"),
		SRSPipelineShaderStage(
			ShaderStage::Fragment,
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

void CClusteredForwardRenderingPath::Draw(const SWorldView& InView)
{
	ZE_ASSERT(InView.TargetRT);
	if(!InView.TargetRT)
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
		ColorInfos.Format = InView.TargetRT->GetCreateInfo().Format;
		ColorInfos.Width = InView.TargetRT->GetCreateInfo().Width;
		ColorInfos.Height = InView.TargetRT->GetCreateInfo().Height;
		ColorInfos.Usage = ERSTextureUsageFlagBits::RenderTarget | ERSTextureUsageFlagBits::Sampled;

		SRenderPassTextureInfos DepthInfos;
		DepthInfos.Format = EFormat::D24UnormS8Uint;
		DepthInfos.Width = InView.TargetRT->GetCreateInfo().Width;
		DepthInfos.Height = InView.TargetRT->GetCreateInfo().Height;

		InData.Color = InRenderPass.CreateTexture(ColorInfos);
		InData.Depth = InRenderPass.CreateTexture(DepthInfos);
		InData.Backbuffer = InRenderPass.CreateRetainedTexture(InView.TargetRT);

		InRenderPass.Write(InData.Color);
		InRenderPass.Write(InData.Depth);
	},
	[&](IRenderSystemContext* InContext, const SBasePass& InData)
	{
		InContext->SetScissors({ InView.Scissor });
		InContext->SetViewports({ InView.Viewport });

		InView.RenderPassRendererMap.at(ERenderPassFlagBits::BasePass).Draw(*InContext);

	});

	/** POST PROCESS */
	AddBarrelDistortionPass(Graph, BasePass.Color, BasePass.Backbuffer);

	Graph.Compile();
	Graph.Execute();
}

}