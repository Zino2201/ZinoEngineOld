#include "VulkanShader.h"
#include "VulkanDevice.h"

CVulkanShader::CVulkanShader(CVulkanDevice* InDevice, vk::ShaderStageFlagBits InStageFlag, 
	const std::vector<uint8_t>& InData)
	: Device(InDevice), StageFlag(InStageFlag)
{
	/** Create shader module */
	vk::ShaderModuleCreateInfo CreateInfos(
		vk::ShaderModuleCreateFlags(),
		InData.size(),
		reinterpret_cast<const uint32_t*>(InData.data()));

	ShaderModule = InDevice->GetDevice().createShaderModuleUnique(CreateInfos);
	if(!ShaderModule)
		LOG(ELogSeverity::Fatal, "Failed to create shader module")
}

CVulkanShader::~CVulkanShader() {}