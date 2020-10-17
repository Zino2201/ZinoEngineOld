#include "BasePass.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ze::renderer
{

CBasePassShaderVS::CBasePassShaderVS(const gfx::shaders::CShaderType* InType, const gfx::shaders::ShaderCompilerOutput& InOutput)
	: gfx::shaders::CBasicShader(InType, InOutput) {}

CBasePassShaderFS::CBasePassShaderFS(const gfx::shaders::CShaderType* InType, const gfx::shaders::ShaderCompilerOutput& InOutput)
	: gfx::shaders::CBasicShader(InType, InOutput) {}

IMPLEMENT_SHADER(CBasePassShaderVS, "BasePassVS", 
	"ClusteredForward/BasePassVS.hlsl", "Main", gfx::shaders::ShaderStage::Vertex);

IMPLEMENT_SHADER(CBasePassShaderFS, "BasePassFS",
	"ClusteredForward/BasePassFS.hlsl", "Main", gfx::shaders::ShaderStage::Fragment);

}