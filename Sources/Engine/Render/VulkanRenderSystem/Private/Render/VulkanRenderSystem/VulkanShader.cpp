#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"

CVulkanShader::CVulkanShader(CVulkanDevice* InDevice, 
	const EShaderStage& InStage, 
	const uint64_t& InBytecodeSize, 
	const void* InBytecode, 
	const SShaderParameterMap& InParameterMap, 
	const SRSResourceCreateInfo& InCreateInfo)
	: CVulkanDeviceResource(InDevice), CRSShader(InStage, InBytecodeSize, InBytecode,
		InParameterMap,
		InCreateInfo)
{
	/** Create shader module */
	vk::ShaderModuleCreateInfo CreateInfos(
		vk::ShaderModuleCreateFlags(),
		InBytecodeSize * sizeof(uint32_t),
		reinterpret_cast<const uint32_t*>(InBytecode));

	ShaderModule = Device->GetDevice().createShaderModuleUnique(CreateInfos).value;
	if (!ShaderModule)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create shader module");
}

CRSShader* CVulkanRenderSystem::CreateShader(
	const EShaderStage& InStage,
	const uint64_t& InBytecodeSize,
	const void* InBytecode,
	const SShaderParameterMap& InParameterMap, 
	const SRSResourceCreateInfo& InCreateInfo) const
{
	return new CVulkanShader(Device.get(),
		InStage,
		InBytecodeSize,
		InBytecode,
		InParameterMap,
		InCreateInfo);
}

vk::ShaderStageFlagBits VulkanUtil::ShaderStageToVkShaderStage(const EShaderStage& InShader)
{
	switch(InShader)
	{
	default:
	case EShaderStage::Vertex:
		return vk::ShaderStageFlagBits::eVertex;
	case EShaderStage::Fragment:
		return vk::ShaderStageFlagBits::eFragment;
	}
}