#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"

CVulkanShader::CVulkanShader(CVulkanDevice& InDevice, 
	const SRSShaderCreateInfo& InCreateInfo)
	: CVulkanDeviceResource(InDevice), CRSShader(InCreateInfo)
{
	/** Create shader module */
	vk::ShaderModuleCreateInfo CreateInfos(
		vk::ShaderModuleCreateFlags(),
		InCreateInfo.BytecodeSize * sizeof(uint32_t),
		reinterpret_cast<const uint32_t*>(InCreateInfo.Bytecode));

	ShaderModule = Device.GetDevice().createShaderModuleUnique(CreateInfos).value;
	if (!ShaderModule)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create shader module");
}

TOwnerPtr<CRSShader> CVulkanRenderSystem::CreateShader(
	const SRSShaderCreateInfo& InCreateInfo) const
{
	return new CVulkanShader(*Device.get(),
		InCreateInfo);
}