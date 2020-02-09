#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanDevice;

/**
 * Vulkan shader
 */
class CVulkanShader : public CRenderSystemShader
{
public:
	CVulkanShader(CVulkanDevice* InDevice,
		void* InData,
		size_t InDataSize,
		const EShaderStage& InShaderStage);
	virtual ~CVulkanShader();

	const vk::ShaderModule& GetShaderModule() const { return *ShaderModule; }
	const vk::ShaderStageFlagBits& GetStageFlag() const { return StageFlag; }
	const char* GetMain() const { return "main"; }
	const std::vector<uint8_t>& GetData() const { return Data; }
private:
	CVulkanDevice* Device;
	vk::UniqueShaderModule ShaderModule;
	vk::ShaderStageFlagBits StageFlag;
	std::vector<uint8_t> Data;
};