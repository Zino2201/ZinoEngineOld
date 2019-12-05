#pragma once

#include "Core/Asset.h"
#include "RenderCore.h"

class IGraphicsPipeline;
class IShader;
class IDeviceResource;
class ISampler;

/**
 * Material base class
 */
class CMaterial : public IAsset
{
public:
	virtual void Load(const std::string& InPath) override;

	const std::shared_ptr<IGraphicsPipeline>& GetPipeline() const { return Pipeline; }

	void SetShaderAttributeResource(const EShaderStage& InStage,
		const std::string& InName, IDeviceResource* InResource);
private:
	std::vector<SShaderAttribute> ParseShaderJson(const EShaderStage& InStage,
		const std::string& InPath);
private:
	std::shared_ptr<IGraphicsPipeline> Pipeline;
	std::map<EShaderStage, std::shared_ptr<IShader>> ShaderMap;
	std::map<EShaderStage, std::vector<SShaderAttribute>> ShaderAttributeMap;
};