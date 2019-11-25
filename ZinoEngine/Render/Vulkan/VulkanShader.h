#pragma once

#include "VulkanCore.h"
#include "Render/Shader.h"

class CVulkanDevice;

/**
 * Vulkan shader
 */
class CVulkanShader : public IShader
{
public:
	CVulkanShader(CVulkanDevice* InDevice,
		const std::vector<uint8_t>& InData,
		const EShaderStage& InShaderStage);
	virtual ~CVulkanShader();

	const vk::ShaderModule& GetShaderModule() const { return *ShaderModule; }
	const vk::ShaderStageFlagBits& GetStageFlag() const { return StageFlag; }
	const char* GetMain() const { return "main"; }
private:
	CVulkanDevice* Device;
	vk::UniqueShaderModule ShaderModule;
	vk::ShaderStageFlagBits StageFlag;
};