#pragma once

#include "VulkanCore.h"

class CVulkanDevice;

/**
 * Vulkan shader
 */
class CVulkanShader : public std::enable_shared_from_this<CVulkanShader>
{
public:
	CVulkanShader(CVulkanDevice* InDevice,
		vk::ShaderStageFlagBits InStageFlag,
		const std::vector<uint8_t>& InData);
	virtual ~CVulkanShader();

	std::shared_ptr<CVulkanShader> Get() { return shared_from_this(); }
	const vk::ShaderModule& GetShaderModule() const { return *ShaderModule; }
	const vk::ShaderStageFlagBits& GetStageFlag() const { return StageFlag; }
	const char* GetMain() const { return "main"; }
private:
	CVulkanDevice* Device;
	vk::UniqueShaderModule ShaderModule;
	vk::ShaderStageFlagBits StageFlag;
};