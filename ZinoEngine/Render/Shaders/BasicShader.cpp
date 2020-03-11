#include "BasicShader.h"

std::map<std::string, CShaderPtr> CBasicShaderClass::BasicShaderMap;
std::map<std::string, CShaderClass*> CBasicShaderClass::BasicShaderClassesMap;

CBasicShaderClass::CBasicShaderClass(const std::string& InName, 
	const std::string& InFilename, EShaderStage InStage, InstantiateFunctionType InInstantiateFunction)
	: CShaderClass(InName,
		InFilename, InStage, InInstantiateFunction) 
{
	BasicShaderClassesMap.insert(std::make_pair(InName, this));
}

void CBasicShaderClass::InstantiateBasicShaders()
{
	/** Instantiate all basic shader class */
	for(const auto& [Name, Class] : BasicShaderClassesMap)
	{
		BasicShaderMap[Class->GetName()] = InstantiateShaderSync<CShader>(Name);
	}
}

void CBasicShaderClass::DestroyBasicShaders()
{
	LOG(ELogSeverity::Debug, "Destroying basic shaders")

	BasicShaderMap.clear();

	for(const auto& [Name, Class] : BasicShaderClassesMap)
	{
		Class->ClearShaders();
	}
}

CBasicShader::CBasicShader(CShaderClass* InClass,
	const SCompiledShaderData& InData)
	: CShader(InClass, InData)
{
	
}

CBasicShader::~CBasicShader() {}