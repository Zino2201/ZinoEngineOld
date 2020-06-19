#include "BasePass.h"
#include "Renderer/MeshRenderPass.inl"

namespace ZE::Renderer
{

CBasePassShaderVS::CBasePassShaderVS(const CShaderType* InType, const SShaderCompilerOutput& InOutput)
	: CBasicShader(InType, InOutput) {}

CBasePassShaderFS::CBasePassShaderFS(const CShaderType* InType, const SShaderCompilerOutput& InOutput)
	: CBasicShader(InType, InOutput) {}

IMPLEMENT_SHADER(CBasePassShaderVS, "BasePassVS", 
	"/ClusteredForward/BasePassVS.hlsl", "Main", EShaderStage::Vertex);

IMPLEMENT_SHADER(CBasePassShaderFS, "BasePassFS",
	"/ClusteredForward/BasePassFS.hlsl", "Main", EShaderStage::Fragment);

void CBasePass::Process(CWorldProxy* InWorldProxy,
	CRenderableComponentProxy* InProxy,
	const CMeshCollection& InCollection,
	const size_t& InInstanceIdx)
{
	/**
	 * Get shaders
	 */
	SShaderArray<CBasePassShaderVS, CBasePassShaderFS> Shaders;

	/**
	 * Build mesh draw command
	 */
	BuildDrawCommand(InWorldProxy,
		InProxy,
		InCollection,
		InInstanceIdx,
		Shaders);
}

IMPLEMENT_MESH_RENDER_PASS(CBasePass, EMeshRenderPass::BasePass);

}