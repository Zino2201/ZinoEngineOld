#include "BasePass.h"

namespace ZE::Renderer
{

CBasePassShaderVS::CBasePassShaderVS(const SShaderCompilerOutput& InOutput)
	: CBasicShader(InOutput) {}

CBasePassShaderFS::CBasePassShaderFS(const SShaderCompilerOutput& InOutput)
	: CBasicShader(InOutput) {}

IMPLEMENT_SHADER(CBasePassShaderVS, "BasePassShaderVS", 
	"ClusteredForward/BasePassVS.hlsl", "Main", EShaderStage::Vertex);

IMPLEMENT_SHADER(CBasePassShaderFS, "BasePassShaderFS",
	"ClusteredForward/BasePassFS.hlsl", "Main", EShaderStage::Fragment);

}