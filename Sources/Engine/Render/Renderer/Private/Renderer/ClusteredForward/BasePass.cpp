#include "BasePass.h"
#include "Render/RenderSystem/RenderSystemResources.h"

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

}