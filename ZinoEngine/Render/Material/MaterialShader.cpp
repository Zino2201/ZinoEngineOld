#include "MaterialShader.h"

CMaterialShaderClass::CMaterialShaderClass(const std::string& InName,
	const std::string& InFilename, EShaderStage InStage, 
	InstantiateFunctionType InInstantiateFunction) : CShaderClass(InName,
		InFilename, InStage, InInstantiateFunction) {}

CMaterialShader::CMaterialShader(CShaderClass* InClass,
	const SCompiledShaderData& InData)
	: CShader(InClass, InData) {}
CMaterialShader::~CMaterialShader() {}