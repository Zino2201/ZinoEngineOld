#pragma once

#include "Core/Asset.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class IRenderSystemGraphicsPipeline;
class CRenderSystemShader;
class IDeviceResource;
class CRenderSystemSampler;
class IRenderSystemUniformBuffer;
class IShaderAttributesManager;

/**
 * Material render data
 */
class CMaterialRenderData final : public CRenderResource
{
	friend class CMaterial;

public:
	void Init(CMaterial* InMaterial);
	virtual void InitRenderThread() override;
	virtual void DestroyRenderThread() override;

	IRenderSystemGraphicsPipeline* GetPipeline() const { return Pipeline.get(); }
	IRenderSystemUniformBuffer* GetUniformBuffer() const { return UniformBuffer.get(); }
private:
	IRenderSystemGraphicsPipelinePtr Pipeline;
	IRenderSystemUniformBufferPtr UniformBuffer;
	CMaterial* Material;
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

	/** test func */
	void SetMaterialUBO(const void* InNewData, const uint64_t& InSize);
	CMaterialRenderData* GetRenderData() const { return RenderData; }
	class CTexture2D* TestTexture;
private:
	std::vector<SShaderParameter> ParseShaderJson(const EShaderStage& InStage,
		const std::string& InPath);
private:
	std::map<EShaderStage, CRenderSystemShaderPtr> ShaderMap;
	std::map<EShaderStage, std::vector<SShaderParameter>> ShaderParameterMap;
	CMaterialRenderData* RenderData;
};