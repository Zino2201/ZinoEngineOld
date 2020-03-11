#pragma once

#include "Render/Shader.h"

/**
 * Base class for shader that require materials data
 */
class CMaterialShaderClass : public CShaderClass
{
public:
	CMaterialShaderClass(const std::string& InName,
		const std::string& InFilename,
		EShaderStage InStage, InstantiateFunctionType InInstantiateFunction);

	/**
	 * Compile and return a new instance of this materal shader
	 */
	CShader* CompileShader();
};

/**
 * A shader to be used for a material
 */
class CMaterialShader : public CShader
{
public:
	CMaterialShader(CShaderClass* InClass,
		const SCompiledShaderData& InData);
	virtual ~CMaterialShader();
};