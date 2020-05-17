#include "Render/Shader/Shader.h"

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
	return InstantiateFunc(InOutput);
}

/** Shader */

CShader::CShader(const SShaderCompilerOutput& InOutput)
{
}

/** SHADER MANAGER */

void CShaderManager::AddShaderType(const char* InName, CShaderType* InShaderType)
{
	ShaderMap.insert({ InName, InShaderType });
}

}