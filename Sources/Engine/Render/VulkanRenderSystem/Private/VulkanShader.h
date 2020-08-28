#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/Resources/Shader.h"

class CVulkanShader : public CVulkanDeviceResource,
	public CRSShader
{
public:
	CVulkanShader(CVulkanDevice& InDevice,
		const SRSShaderCreateInfo& InCreateInfo);

	const vk::ShaderModule& GetShaderModule() const { return *ShaderModule; }
private:
	vk::UniqueShaderModule ShaderModule;
};