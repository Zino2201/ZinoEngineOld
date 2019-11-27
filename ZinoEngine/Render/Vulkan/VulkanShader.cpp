#include "VulkanShader.h"
#include "VulkanDevice.h"

CVulkanShader::CVulkanShader(CVulkanDevice* InDevice, 
	const std::vector<uint8_t>& InData,
	const EShaderStage& InShaderStage)
	: IShader(InData, InShaderStage), 
	Device(InDevice), 
	StageFlag(VulkanUtil::ShaderStageToVkShaderStage(InShaderStage))
{
	/** Create shader module */
	vk::ShaderModuleCreateInfo CreateInfos(
		vk::ShaderModuleCreateFlags(),
		InData.size(),
		reinterpret_cast<const uint32_t*>(InData.data()));

	ShaderModule = InDevice->GetDevice().createShaderModuleUnique(CreateInfos).value;
	if(!ShaderModule)
		LOG(ELogSeverity::Fatal, "Failed to create shader module")
}

CVulkanShader::~CVulkanShader() {}