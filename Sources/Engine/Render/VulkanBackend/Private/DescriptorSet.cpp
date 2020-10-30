#include "DescriptorSet.h"
#include "PipelineLayout.h"
#include "Device.h"
#include "Buffer.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include "Texture.h"
#include "Sampler.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, DescriptorSet> sets;
vk::Result last_set_result;

std::pair<Result, ResourceHandle> VulkanBackend::descriptor_set_create(const DescriptorSetCreateInfo& in_create_info)
{
	ResourceHandle handle;

	DescriptorSet set(*device, in_create_info);
	if(set.is_valid())
	{
		handle = create_resource_handle(ResourceType::DescriptorSet,
			static_cast<VkDescriptorSet>(set.get_set()), in_create_info);
		sets.insert({ handle, std::move(set) });
	}
	
	return { convert_vk_result(last_set_result), handle };
}

void VulkanBackend::descriptor_set_destroy(const ResourceHandle& in_handle)
{
	sets.erase(in_handle);
}

DescriptorSet::DescriptorSet(Device& in_device, const DescriptorSetCreateInfo& in_create_info)
	: device(in_device), layout(nullptr), set_idx(-1)
{
	layout = PipelineLayout::get(in_create_info.pipeline_layout);
	ZE_CHECKF(layout, "Invalid pipeline layout given to descriptor_set_create");

	uint64_t descs_hash = 0;
	for(const auto& descriptor : in_create_info.descriptors)
		ze::hash_combine(descs_hash, descriptor.type);

	set_idx = layout->get_set_from_descriptors_hash(descs_hash);
	set = layout->allocate_set(set_idx);
	last_set_result = vk::Result::eSuccess;

	std::vector<vk::WriteDescriptorSet> write_sets;
	write_sets.reserve(in_create_info.descriptors.size());

	std::vector<vk::DescriptorBufferInfo> buffer_infos;
	buffer_infos.reserve(64);
	std::vector<vk::DescriptorImageInfo> image_infos;
	image_infos.reserve(64);

	for(const auto& desc : in_create_info.descriptors)
	{
		vk::DescriptorBufferInfo& buffer_info = buffer_infos.emplace_back();
		vk::DescriptorImageInfo& image_info = image_infos.emplace_back();
		switch(desc.type)
		{
		case DescriptorType::Sampler:
		{
			DescriptorTextureInfo image = std::get<DescriptorTextureInfo>(desc.info);
			Sampler* sampler = Sampler::get(image.handle);
			ZE_CHECKF(sampler, "Invalid sampler given to descriptor_set_create"); 
			image_info.setSampler(sampler->get_sampler());
			write_sets.emplace_back(
				set,
				desc.dst_binding,
				0,
				1,
				convert_descriptor_type(desc.type),
				&image_info,
				nullptr);
			break;
		}
		case DescriptorType::InputAttachment:
		case DescriptorType::SampledTexture:
		{
			DescriptorTextureInfo image = std::get<DescriptorTextureInfo>(desc.info);
			TextureView* view = TextureView::get(image.handle);
			ZE_CHECKF(view, "Invalid texture view given to descriptor_set_create"); 
			image_info.setImageView(view->get_image_view());
			image_info.setImageLayout(convert_texture_layout(image.layout));
			write_sets.emplace_back(
				set,
				desc.dst_binding,
				0,
				1,
				convert_descriptor_type(desc.type),
				&image_info,
				nullptr);
			break;
		}
		case DescriptorType::StorageBuffer:
		case DescriptorType::UniformBuffer:
		{
			DescriptorBufferInfo buffer = std::get<DescriptorBufferInfo>(desc.info);
			Buffer* buf = Buffer::get(buffer.buffer);
			ZE_CHECKF(buf, "Invalid buffer given to descriptor binding {}", desc.dst_binding);
			buffer_info.setBuffer(buf->get_buffer());
			buffer_info.setOffset(buffer.offset);
			buffer_info.setRange(buffer.range);
			write_sets.emplace_back(
				set,
				desc.dst_binding,
				0,
				1,
				convert_descriptor_type(desc.type),
				nullptr,
				&buffer_info);
			break;
		}
		}
	}

	device.get_device().updateDescriptorSets(
		write_sets,
		{});
}

DescriptorSet::~DescriptorSet()
{
	layout->free_set(set_idx, set);
}

DescriptorSet* DescriptorSet::get(const ResourceHandle& in_handle)
{
	auto set = sets.find(in_handle);

	if(set != sets.end())
		return &set->second;

	return nullptr;
}

}