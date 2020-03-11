#pragma once

#include "Core/Asset.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Shader.h"

class IRenderSystemGraphicsPipeline;
class CRenderSystemShader;
class IDeviceResource;
class CRenderSystemSampler;
class IRenderSystemUniformBuffer;
class IShaderAttributesManager;
class IRenderCommandContext;

struct SMaterialShader
{
	EShaderStage Stage;
	std::string Type;
	std::string Shader;
};

/**
 * Material render data
 */
class CMaterialRenderData final : public CRenderResource
{
	friend class CMaterial;

public:
	void Init(CMaterial* InMaterial,
		const std::vector<SMaterialShader>& InShaders);
	virtual void InitRenderThread() override;
	virtual void DestroyRenderThread() override;

	/**
	 * Bind this material resources to the MATERIAL_SET
	 */
	void Bind(IRenderCommandContext* InContext);

	IRenderSystemGraphicsPipeline* GetPipeline() const { return Pipeline.get(); }
	const CShaderMap& GetShaderMap() const { return ShaderMap; }
	CShaderParameterUniformBuffer& GetScalars() { return Scalars; }
	CShaderParameterUniformBuffer& GetVec3s() { return Vec3s; }
private:
	CMaterial* Material;
	IRenderSystemGraphicsPipelinePtr Pipeline;

	/** Shader map */
	CShaderMap ShaderMap;
	std::vector<SMaterialShader> Shaders;

	/** Resources */
	CShaderParameterUniformBuffer Scalars;
	CShaderParameterUniformBuffer Vec3s;
};

/**
 * Material base class
 * TODO: recode
 */
class CMaterial : public IAsset
{
	friend class CMaterialRenderData;

public:
	~CMaterial();

	virtual void Load(const std::string& InPath) override;

	CMaterialRenderData* GetRenderData() const { return RenderData; }
private:
	std::map<EShaderStage, CRenderSystemShaderPtr> ShaderMap;
	std::map<EShaderStage, std::vector<SShaderParameter>> ShaderParameterMap;
	CMaterialRenderData* RenderData;
};