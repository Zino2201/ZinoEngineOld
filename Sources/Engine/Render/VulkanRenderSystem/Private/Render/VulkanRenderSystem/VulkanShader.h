#pragma once

#include "VulkanCore.h"

class CVulkanShader : public CVulkanDeviceResource,
	public CRSShader
{
public:
	CVulkanShader(CVulkanDevice* InDevice,
		const EShaderStage& InStage,
		const uint64_t& InBytecodeSize,
		const void* InBytecode,
		const SShaderParameterMap& InParameterMap,
		const SRSResourceCreateInfo& InCreateInfo);

	const vk::ShaderModule& GetShaderModule() const { return *ShaderModule; }
private:
	vk::UniqueShaderModule ShaderModule;
};