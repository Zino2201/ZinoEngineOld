#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "IO/IOUtils.h"

CVulkanShader::CVulkanShader(CVulkanDevice* InDevice, 
	void* InData,
	size_t InDataSize,
	const EShaderStage& InShaderStage)
	: CRenderSystemShader(InData, InDataSize, InShaderStage), 
	Device(InDevice), 
	StageFlag(VulkanUtil::ShaderStageToVkShaderStage(InShaderStage))
{
	Data.resize(InDataSize / 4);
	memcpy(Data.data(), InData, Data.size());

	/** Create shader module */
	vk::ShaderModuleCreateInfo CreateInfos(
		vk::ShaderModuleCreateFlags(),
		InDataSize,
		reinterpret_cast<const uint32_t*>(InData));

	ShaderModule = InDevice->GetDevice().createShaderModuleUnique(CreateInfos).value;
	if(!ShaderModule)
		LOG(ELogSeverity::Fatal, "Failed to create shader module")
}

CVulkanShader::~CVulkanShader() {}