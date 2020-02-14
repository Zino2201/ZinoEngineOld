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

	IRenderSystemGraphicsPipeline* GetPipeline() const { return Pipeline.get(); }
	const std::shared_ptr<IShaderAttributesManager>& GetShaderAttributesManager() const { return ShaderAttributesManager; }
private:
	std::shared_ptr<IShaderAttributesManager> ShaderAttributesManager;
	std::map<SShaderAttribute, IRenderSystemUniformBufferPtr> AttributeBufferMap;
	IRenderSystemGraphicsPipelinePtr Pipeline;
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
	virtual void Load(const std::string& InPath) override;

	void SetShaderAttributeResource(const EShaderStage& InStage,
		const std::string& InName, IRenderSystemResource* InResource);

	void SetUniformBuffer(const std::string& InName, void* InData);

	/**
	 * Set a float member variable
	 * Only from the material set
	 */
	void SetFloat(const std::string& InMemberName, const float& InData);
	
	/**
	 * Set a vector3 member variable
	 */
	void SetVec3(const std::string& InMemberName, const glm::vec3& InData);
	
	/**
	 * Set texture
	 * WARN: Should not be called inside a command buffer !
	 */
	void SetTexture(const EShaderStage& InStage, const std::string& InName,
		IRenderSystemResource* InDeviceResource);

	CMaterialRenderData* GetRenderData() const { return RenderData.get(); }
private:
	std::vector<SShaderAttribute> ParseShaderJson(const EShaderStage& InStage,
		const std::string& InPath);

	/**
	 * Set a uniform data
	 */
	void SetUniformData(const std::string& InName,
		const void* InData, uint64_t InOffset = 0, uint64_t InSize = 0);
private:
	std::map<EShaderStage, CRenderSystemShaderPtr> ShaderMap;
	std::map<EShaderStage, std::vector<SShaderAttribute>> ShaderAttributeMap;
	std::unique_ptr<CMaterialRenderData> RenderData;
};