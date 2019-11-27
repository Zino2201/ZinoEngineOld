#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"

CVulkanPipelineLayout::CVulkanPipelineLayout(CVulkanDevice* InDevice) :
	CVulkanDeviceResource(InDevice)
{
	vk::PipelineLayoutCreateInfo CreateInfos(
		vk::PipelineLayoutCreateFlags(),
		0,
		nullptr,
		0,
		nullptr);

	PipelineLayout = Device->GetDevice().createPipelineLayoutUnique(CreateInfos).value;
	if (!PipelineLayout)
		LOG(ELogSeverity::Fatal, "Failed to create pipeline layout")
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() {}