#include "EngineCore.h"
#include "Render/Shader/Shader.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/Resources/Shader.h"
#include "Shader/ShaderCore.h"

namespace ze::gfx::shaders
{

/** Shader Type */
CShaderType::CShaderType(const char* InName, const char* InFilename, const char* InEntryPoint,
	ShaderStage InStage,
	InstantiateFunctionType InFunc)
	: Name(InName), Filename(InFilename), EntryPoint(InEntryPoint), Stage(InStage), 
	InstantiateFunc(InFunc)
{
	CShaderManager::Get().AddShaderType(InName, this);
}

CShader* CShaderType::InstantiateShader(const ShaderCompilerOutput& InOutput) const
{
	return InstantiateFunc(this, InOutput);
}

/** Shader */

CShader::CShader(const CShaderType* InShaderType, const ShaderCompilerOutput& InOutput)
{
	Shader = GRenderSystem->CreateShader({ InShaderType->GetStage(),
		InOutput.bytecode.size(),
		InOutput.bytecode.data(),
		InOutput.reflection_data.parameter_map });
	if(!Shader)
		ze::logger::error("Failed to create RS shader");
}

CShader::~CShader() = default;

/** SHADER MANAGER */

void CShaderManager::AddShaderType(const char* InName, CShaderType* InShaderType)
{
	ShaderMap.insert({ InName, InShaderType });
}

}