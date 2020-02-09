#include "BasicShader.h"

std::map<std::string, CShader*> g_BasicShaderMap;

CBasicShaderClass::CBasicShaderClass(const std::string& InName, 
	const std::string& InFilename, EShaderStage InStage, InstantiateFunctionType InInstantiateFunction)
	: CShaderClass(InName,
		InFilename, InStage, InInstantiateFunction) {}

void CBasicShaderClass::InstantiateBasicShaders()
{
	/** Instantiate all shaders class */
	for(const auto& [Name, Class] : CShaderClass::GetShaderClassMap())
	{
		g_BasicShaderMap[Class->GetName()] = Class->InstantiateShader();
	}
}

CBasicShader::CBasicShader(CShaderClass* InClass)
	: CShader(InClass)
{
	
}

CBasicShader::~CBasicShader() {}