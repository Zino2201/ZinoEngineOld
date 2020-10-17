#include "Renderer/MeshRendering/RenderPassDrawcallFactory.h"
#include "Render/Shader/BasicShader.h"
#include "Renderer/WorldView.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ze::renderer
{

class CBasePassDrawcallFactory : public CRenderPassDrawcallFactory
{
public:
	CBasePassDrawcallFactory(CWorldProxy& InWorld) : CRenderPassDrawcallFactory(InWorld)
	{
		std::vector<SVertexInputBindingDescription> BindingDescriptions =
		{
				SVertexInputBindingDescription(0, 12 * 2,
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

	void ProcessMesh(const SWorldView& InWorldView,
		const CRenderableComponentProxy& InProxy,
		const SMesh& InMesh) override
	{
		ComputeDrawcalls(InMesh, Test, 
			{
				{ SMeshDrawcallShaderBinding(0, 0, gfx::shaders::ShaderParameterType::UniformBuffer, InWorldView.TEST_ViewUBO.GetBuffer()) },
				{ SMeshDrawcallShaderBinding(0, 1, gfx::shaders::ShaderParameterType::UniformBuffer, InProxy.TEST_PerInstanceUBO.GetBuffer()) },
			});
	}
private:
	SRSGraphicsPipeline Test;
};

DEFINE_RENDER_PASS_DRAWCALL_FACTORY(ERenderPassFlagBits::BasePass, CBasePassDrawcallFactory);

}