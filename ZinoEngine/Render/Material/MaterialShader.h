#pragma once

#include "Render/Shader.h"

/**
 * Material shader class
 */
class CMaterialShaderClass : public CShaderClass
{
public:
	CMaterialShaderClass(const std::string& InName,
		const std::string& InFilename,
		EShaderStage InStage, InstantiateFunctionType InInstantiateFunction);
};

/**
 * A shader to be used for a material
 */
class CMaterialShader : public CShader
{
public:
	CMaterialShader(CShaderClass* InClass);
	virtual ~CMaterialShader();
};