#pragma once

#include "RenderCore.h"

class IDeviceResource;
class IPipeline;

struct SShaderAttributesManagerInfo
{
	IPipeline* Pipeline;

	SShaderAttributesManagerInfo(IPipeline* InPipeline) :
		Pipeline(InPipeline) {}
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
		IDeviceResource* InResource) = 0;
};