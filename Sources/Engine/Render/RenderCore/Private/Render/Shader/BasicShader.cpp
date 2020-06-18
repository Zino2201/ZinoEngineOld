#include "Render/Shader/BasicShader.h"
#include "Shader/ShaderCompiler.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"

DECLARE_LOG_CATEGORY(BasicShader);

namespace ZE
{

CBasicShaderType::CBasicShaderType(const char* InName, const char* InFilename, 
	const char* InEntryPoint, EShaderStage InStage,
	InstantiateFunctionType InFunc) :
	CShaderType(InName, InFilename, InEntryPoint, InStage, InFunc)
{
	CBasicShaderManager::Get().AddShaderType(this);
}

CBasicShaderType::~CBasicShaderType()
{
	CBasicShaderManager::Get().RemoveShaderType(this);
}

void CBasicShaderManager::AddShaderType(CBasicShaderType* InShaderType)
{
	ShaderTypes.emplace_back(InShaderType);
}

void CBasicShaderManager::CompileShaders()
{
	ShaderMap.clear();

	for(const auto& ShaderType : ShaderTypes)
	{
		Compile:

		auto Future = CGlobalShaderCompiler::Get().CompileShader(
			ShaderType->GetStage(),
			ShaderType->GetFilename(),
			ShaderType->GetEntryPoint(),
			EShaderCompilerTarget::VulkanSpirV);

		auto Output = Future.get();
		if(!Output.bSucceed)
		{
			LOG(ELogSeverity::FatalRetryDebug, BasicShader, 
				"Failed to compile basic shader %s (%s), can't continue.\n\n%s",
				ShaderType->GetName(), ShaderType->GetFilename(), Output.ErrMsg.c_str());

			goto Compile;
		}
		else
		{
			CShader* Shader = ShaderType->InstantiateShader(Output);
			if(!Shader)
				LOG(ELogSeverity::Fatal, BasicShader, "Failed to instantiate basic shader %s (%s), exiting",
					ShaderType->GetName(), ShaderType->GetFilename());

			ShaderMap.insert( { ShaderType->GetName(), Shader });
		}
	}
}

void CBasicShaderManager::DestroyAll()
{
	ShaderMap.clear();
}

void CBasicShaderManager::RemoveShaderType(CBasicShaderType* InShaderType)
{
	ShaderMap.erase(InShaderType->GetName());
	ShaderTypes.erase(std::remove(ShaderTypes.begin(), ShaderTypes.end(), 
		InShaderType));
}

}