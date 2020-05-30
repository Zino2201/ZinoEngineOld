#pragma once

#include "Render/Shader/BasicShader.h"
#include "Renderer/MeshRenderPass.h"

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

class CBasePass : public CMeshRenderPass
{
public:
	CBasePass(EMeshRenderPass InRenderPass) : CMeshRenderPass(InRenderPass) {}

	void Process(CWorldProxy* InWorldProxy,
		CRenderableComponentProxy* InProxy,
		const CMeshCollection& InCollection,
		const size_t& InInstanceIdx) override;
};

}