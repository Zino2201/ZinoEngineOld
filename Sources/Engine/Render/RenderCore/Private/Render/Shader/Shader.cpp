#include "Render/Shader/Shader.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/Resources/Shader.h"

DECLARE_LOG_CATEGORY(Shader);

namespace ZE
{

/** Shader Type */
CShaderType::CShaderType(const char* InName, const char* InFilename, const char* InEntryPoint,
	EShaderStage InStage,
	InstantiateFunctionType InFunc)
	: Name(InName), Filename(InFilename), EntryPoint(InEntryPoint), Stage(InStage), 
	InstantiateFunc(InFunc)
{
	CShaderManager::Get().AddShaderType(InName, this);
}

CShader* CShaderType::InstantiateShader(const SShaderCompilerOutput& InOutput) const
{
	return InstantiateFunc(this, InOutput);
}

/** Shader */

CShader::CShader(const CShaderType* InShaderType, const SShaderCompilerOutput& InOutput)
{
	Shader = GRenderSystem->CreateShader({ InShaderType->GetStage(),
		InOutput.Bytecode.size(),
		InOutput.Bytecode.data(),
		InOutput.ReflectionData.ParameterMap });
	if(!Shader)
		LOG(ELogSeverity::Error, Shader, "Failed to create RS shader");
}

CShader::~CShader() = default;

/** SHADER MANAGER */

void CShaderManager::AddShaderType(const char* InName, CShaderType* InShaderType)
{
	ShaderMap.insert({ InName, InShaderType });
}

}