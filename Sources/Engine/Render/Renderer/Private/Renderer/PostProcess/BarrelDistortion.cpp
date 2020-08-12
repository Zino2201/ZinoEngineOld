#include "BarrelDistortion.h"
#include "Render/Shader/BasicShader.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/Resources/Sampler.h"
#include "Render/UniformBuffer.h"

namespace ZE::Renderer
{

class CBarrelDistortionVS : public CBasicShader
{
	DECLARE_SHADER(CBarrelDistortionVS, CBasicShader)

public:
	CBarrelDistortionVS(const CShaderType* InType, const SShaderCompilerOutput& InOutput) 
		: CBasicShader(InType, InOutput) {}
};

class CBarrelDistortionFS : public CBasicShader
{
	DECLARE_SHADER(CBarrelDistortionFS, CBasicShader)

public:
	CBarrelDistortionFS(const CShaderType* InType, const SShaderCompilerOutput& InOutput) 
		: CBasicShader(InType, InOutput) {}
};

IMPLEMENT_SHADER(CBarrelDistortionVS, "BarrelDistortionVS", 
	"PostProcess/BarrelDistortionVS.hlsl", "Main", EShaderStage::Vertex);

IMPLEMENT_SHADER(CBarrelDistortionFS, "BarrelDistortionFS",
	"PostProcess/BarrelDistortionFS.hlsl", "Main", EShaderStage::Fragment);

const SBarrelDistortionData& AddBarrelDistortionPass(CFrameGraph& InFrameGraph,
	const RenderPassResourceID& InColor,
	const RenderPassResourceID& InTarget)
{
	// TODO: Static Sampler State
	static CRSSampler* TEST_SAMPLER = nullptr;

	if(!TEST_SAMPLER)
		TEST_SAMPLER = GRenderSystem->CreateSampler(SRSSamplerCreateInfo());

	return InFrameGraph.AddQuadRenderPass<SBarrelDistortionData>("BarrelDistortion",
		[&](CRenderPass& InRenderPass, SBarrelDistortionData& InData)
		{
			InData.Color = InColor;

			InRenderPass.Read(InColor, ERSRenderPassAttachmentLoadOp::Load);
			InRenderPass.Write(InTarget, ERenderPassResourceLayout::Present);
		},
		[&](IRenderSystemContext* InContext, const SBarrelDistortionData& InData)
		{
			InContext->SetShaderTexture(0, 0, InFrameGraph.GetTexture(InData.Color));
			InContext->SetShaderSampler(0, 1, TEST_SAMPLER);
		},
		SRSPipelineShaderStage(EShaderStage::Vertex,
			CBasicShaderManager::Get().GetShader("BarrelDistortionVS")->GetShader(),
			"Main"),
		SRSPipelineShaderStage(EShaderStage::Fragment,
			CBasicShaderManager::Get().GetShader("BarrelDistortionFS")->GetShader(),
			"Main"));
}

}