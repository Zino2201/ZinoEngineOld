#pragma once

#include "RenderCore.h"

class IRenderSystemResource;
class CRenderSystemPipeline;

struct SShaderAttributesManagerInfo
{
	CRenderSystemPipeline* Pipeline;
	EShaderAttributeFrequency Frequency;

	SShaderAttributesManagerInfo() {}

	SShaderAttributesManagerInfo(CRenderSystemPipeline* InPipeline, EShaderAttributeFrequency
		InFrequency = EShaderAttributeFrequency::PerMaterial) :
		Pipeline(InPipeline), Frequency(InFrequency) {}
};

/**
 * Shader attribute manager base interface
 */
class IShaderAttributesManager
{
public:
	IShaderAttributesManager(const SShaderAttributesManagerInfo& InInfos) {}
	virtual ~IShaderAttributesManager() = default;

	virtual void Set(EShaderStage InStage, const std::string& InName, 
		IRenderSystemResource* InResource) = 0;

	virtual void SetUniformBuffer(const std::string& InName,
		void* InData) = 0;

	virtual const SShaderAttributesManagerInfo& GetInfos() const = 0;
};