#include "VulkanShaderAttributesManager.h"
#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanSampler.h"

CVulkanShaderAttributesManager::CVulkanShaderAttributesManager(
	const SShaderAttributesManagerInfo& InInfos) : IShaderAttributesManager(InInfos),
	Pipeline(static_cast<CVulkanPipeline*>(InInfos.Pipeline))
{
	std::vector<vk::DescriptorSetLayout> SetLayouts(
		g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size(),
		Pipeline->GetDescriptorSetLayout());

	DescriptorSets = g_VulkanRenderSystem->GetDevice()->GetDevice().allocateDescriptorSetsUnique(
		vk::DescriptorSetAllocateInfo(Pipeline->GetDescriptorPool(),
			static_cast<uint32_t>(g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size()),
			SetLayouts.data())).value;
}

CVulkanShaderAttributesManager::~CVulkanShaderAttributesManager() {}

void CVulkanShaderAttributesManager::Set(EShaderStage InStage, const std::string& InName,
	IDeviceResource* InResource)
{
	for(const SShaderAttribute& Attribute : Pipeline->ShaderAttributes)
	{
		if(Attribute.StageFlags == InStage && Attribute.Name == InName)
		{
			for (size_t i = 0; i < g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size(); i++)
			{
				switch(Attribute.Type)
				{
				case EShaderAttributeType::UniformBufferStatic:
				{
					CVulkanBuffer* Buffer = static_cast<CVulkanBuffer*>(InResource);

					vk::DescriptorBufferInfo BufferInfo = vk::DescriptorBufferInfo()
						.setBuffer(Buffer->GetBuffer())
						.setOffset(0)
						.setRange(VK_WHOLE_SIZE);

					vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
						.setDstSet(*DescriptorSets[i])
						.setDstBinding(Attribute.Binding)
						.setDstArrayElement(0)
						.setDescriptorType(vk::DescriptorType::eUniformBuffer)
						.setDescriptorCount(1)
						.setPBufferInfo(&BufferInfo);

					g_VulkanRenderSystem->GetDevice()->GetDevice()
						.updateDescriptorSets(1, &WriteSet, 0, nullptr);
					break;
				}
				case EShaderAttributeType::CombinedImageSampler:
				{
					CVulkanTextureView* TextureView = static_cast<CVulkanTextureView*>(InResource);
					
					vk::DescriptorImageInfo ImageInfo = vk::DescriptorImageInfo()
						.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
						.setImageView(TextureView->GetImageView())
						.setSampler(TextureView->GetVulkanSampler()->GetSampler());

					vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
						.setDstSet(*DescriptorSets[i])
						.setDstBinding(Attribute.Binding)
						.setDstArrayElement(0)
						.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
						.setDescriptorCount(1)
						.setPImageInfo(&ImageInfo);

					g_VulkanRenderSystem->GetDevice()->GetDevice()
						.updateDescriptorSets(1, &WriteSet, 0, nullptr);
					break;
				}
				}
			}

			return;
		}
	}
}