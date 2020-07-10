#pragma once

#include "Render/Shader/BasicShader.h"

namespace ZE::Renderer
{

class CBasePassShaderVS : public CBasicShader
{
	DECLARE_SHADER(CBasePassShaderVS, CBasicShader)

public:
	CBasePassShaderVS(const CShaderType* InType, const SShaderCompilerOutput& InOutput);
};

class CBasePassShaderFS : public CBasicShader
{
	DECLARE_SHADER(CBasePassShaderFS, CBasicShader)

public:
	CBasePassShaderFS(const CShaderType* InType, const SShaderCompilerOutput& InOutput);
};

}