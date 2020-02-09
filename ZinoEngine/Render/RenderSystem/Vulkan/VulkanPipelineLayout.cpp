#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"

CVulkanPipelineLayout::CVulkanPipelineLayout(CVulkanDevice* InDevice,
	const std::vector<vk::DescriptorSetLayout>& InDescriptorSetLayouts) :
	CVulkanDeviceResource(InDevice)
{
	vk::PipelineLayoutCreateInfo CreateInfos(
		vk::PipelineLayoutCreateFlags(),
		static_cast<uint32_t>(InDescriptorSetLayouts.size()),
		InDescriptorSetLayouts.data(),
		0,
		nullptr);

	PipelineLayout = Device->GetDevice().createPipelineLayoutUnique(CreateInfos).value;
	if (!PipelineLayout)
		LOG(ELogSeverity::Fatal, "Failed to create pipeline layout")
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() {}