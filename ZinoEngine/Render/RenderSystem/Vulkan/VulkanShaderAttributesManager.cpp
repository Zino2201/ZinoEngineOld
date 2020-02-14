#include "VulkanShaderAttributesManager.h"
#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanSampler.h"
#include "Core/Engine.h"

CVulkanShaderAttributesManager::CVulkanShaderAttributesManager(
	const SShaderAttributesManagerInfo& InInfos) : IShaderAttributesManager(InInfos),
	Pipeline(static_cast<CVulkanPipeline*>(InInfos.Pipeline)), Infos(InInfos)
{
	std::vector<vk::DescriptorSetLayout> SetLayouts(
		g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size(),
		Pipeline->GetDescriptorSetLayout(InInfos.Frequency));

	DescriptorPool = g_VulkanRenderSystem->GetDevice()->GetDevice().createDescriptorPoolUnique(
		vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			static_cast<uint32_t>(g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size()),
			static_cast<uint32_t>(Pipeline->GetPoolSizes(InInfos.Frequency).size()),
			Pipeline->GetPoolSizes(InInfos.Frequency).data())).value;
	if (!DescriptorPool)
		LOG(ELogSeverity::Fatal, "Failed to create descriptor pool")

	DescriptorSets = g_VulkanRenderSystem->GetDevice()->GetDevice().allocateDescriptorSetsUnique(
		vk::DescriptorSetAllocateInfo(*DescriptorPool,
			static_cast<uint32_t>(g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size()),
			SetLayouts.data())).value;

	for (const auto& ShaderAttribute : Pipeline->GetShaderAttributes(InInfos.Frequency))
	{
		if (ShaderAttribute.Type == EShaderAttributeType::UniformBufferStatic
			&& ShaderAttribute.Frequency == EShaderAttributeFrequency::PerInstance)
		{
			IRenderSystemUniformBufferPtr UniformBuffer =
				g_Engine->GetRenderSystem()->CreateUniformBuffer(
					SRenderSystemUniformBufferInfos(ShaderAttribute.Size));
			AttributeBufferMap.insert(std::make_pair(ShaderAttribute, UniformBuffer));

			Set(ShaderAttribute.StageFlags,
				ShaderAttribute.Name, UniformBuffer->GetBuffer());
		}
	}
}

CVulkanShaderAttributesManager::~CVulkanShaderAttributesManager() { }

void CVulkanShaderAttributesManager::Set(EShaderStage InStage, const std::string& InName,
	IRenderSystemResource* InResource)
{
	for(const SShaderAttribute& Attribute : Pipeline->GetShaderAttributes(Infos.Frequency))
	{
		if(Attribute.StageFlags == InStage && Attribute.Name == InName)
		{
			switch (Attribute.Type)
			{
			case EShaderAttributeType::UniformBufferStatic:
			{
				CVulkanBuffer* Buffer = static_cast<CVulkanBuffer*>(InResource);

				vk::DescriptorBufferInfo BufferInfo = vk::DescriptorBufferInfo()
					.setBuffer(Buffer->GetBuffer())
					.setOffset(0)
					.setRange(VK_WHOLE_SIZE);

				vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
					.setDstSet(*DescriptorSets[0])
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
					.setSampler(static_cast<CVulkanSampler*>(TextureView->GetSampler())->GetSampler());

				vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
					.setDstSet(*DescriptorSets[0])
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

			return;
		}
	}
}

void CVulkanShaderAttributesManager::SetUniformBuffer(const std::string& InName, void* InData)
{
	for (const SShaderAttribute& Attribute : Pipeline->GetShaderAttributes())
	{
		if (Attribute.Name == InName)
		{
			if (Attribute.Type != EShaderAttributeType::UniformBufferStatic)
				continue;

			auto FindResult = AttributeBufferMap.find(Attribute);
			if (FindResult != AttributeBufferMap.end())
			{
				IRenderSystemUniformBufferPtr UniformBuffer = FindResult->second;

				if (UniformBuffer->GetInfos().bUsePersistentMapping)
				{
					memcpy(UniformBuffer->GetMappedMemory(), InData, Attribute.Size);
				}
				else
				{
					void* Dst = UniformBuffer->Map();
					memcpy(Dst, InData, Attribute.Size);
					UniformBuffer->Unmap();
				}

				return;
			}
		}
	}
}