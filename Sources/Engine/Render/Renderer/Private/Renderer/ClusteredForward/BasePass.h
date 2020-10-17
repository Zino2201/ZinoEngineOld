#pragma once

#include "Render/Shader/BasicShader.h"

namespace ze::renderer
{

class CBasePassShaderVS : public gfx::shaders::CBasicShader
{
	DECLARE_SHADER(CBasePassShaderVS, gfx::shaders::CBasicShader)

public:
	CBasePassShaderVS(const gfx::shaders::CShaderType* InType, const gfx::shaders::ShaderCompilerOutput& InOutput);
};

class CBasePassShaderFS : public gfx::shaders::CBasicShader
{
	DECLARE_SHADER(CBasePassShaderFS, gfx::shaders::CBasicShader)

public:
	CBasePassShaderFS(const gfx::shaders::CShaderType* InType, const gfx::shaders::ShaderCompilerOutput& InOutput);
};

}