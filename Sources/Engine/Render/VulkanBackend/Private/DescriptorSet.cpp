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

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> sets;
#endif

vk::Result last_set_result = vk::Result::eSuccess;

std::pair<Result, ResourceHandle> VulkanBackend::descriptor_set_create(const vk::DescriptorSet& in_set)
{
	ResourceHandle handle = create_resource<DescriptorSet>(*device, in_set);
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	sets.insert(handle);
#endif
	return { convert_vk_result(last_set_result), handle };
}

void VulkanBackend::descriptor_set_destroy(const ResourceHandle& in_handle)
{
	delete_resource<DescriptorSet>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	sets.erase(in_handle);
#endif
}

DescriptorSet::DescriptorSet(Device& in_device, const vk::DescriptorSet& in_set)
	: device(in_device), set(in_set), hash(0) {}
DescriptorSet::~DescriptorSet() {}

DescriptorSet* DescriptorSet::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto set = sets.find(in_handle);
	ZE_CHECKF(set != sets.end(), "Invalid descriptor set");
#endif
	return get_resource<DescriptorSet>(in_handle);
}

void DescriptorSet::update(const uint64_t& in_hash, const std::vector<Descriptor>& in_descriptors)
{
	this->hash = in_hash;

	std::vector<vk::WriteDescriptorSet> writes;
	writes.reserve(in_descriptors.size());
 
	std::vector<vk::DescriptorBufferInfo> buffer_infos;
	buffer_infos.reserve(8);
	std::vector<vk::DescriptorImageInfo> image_infos;
	image_infos.reserve(8);

	for(const auto& descriptor : in_descriptors)
	{
		if(descriptor.dst_binding == -1)
			continue;

		vk::DescriptorBufferInfo& buffer_info = buffer_infos.emplace_back();
		vk::DescriptorImageInfo& image_info = image_infos.emplace_back();
		switch(descriptor.type)
		{
		case DescriptorType::Sampler:
		{
			DescriptorTextureInfo image = std::get<DescriptorTextureInfo>(descriptor.info);
			Sampler* sampler = Sampler::get(image.handle);
			ZE_CHECKF(sampler, "Invalid sampler given to descriptor set update()");
			image_info.setSampler(sampler->get_sampler());
			writes.emplace_back(
				set,
				descriptor.dst_binding,
				0,
				1,
				convert_descriptor_type(descriptor.type),
				&image_info,
				nullptr);
			break;
		}
		case DescriptorType::InputAttachment:
		case DescriptorType::SampledTexture:
		{
			DescriptorTextureInfo image = std::get<DescriptorTextureInfo>(descriptor.info);
			TextureView* view = TextureView::get(image.handle);
			ZE_CHECKF(view, "Invalid texture view given to descriptor set update()");
			image_info.setImageView(view->get_image_view());
			image_info.setImageLayout(convert_texture_layout(image.layout));
			writes.emplace_back(
				set,
				descriptor.dst_binding,
				0,
				1,
				convert_descriptor_type(descriptor.type),
				&image_info,
				nullptr);
			break;
		}
		case DescriptorType::StorageBuffer:
		case DescriptorType::UniformBuffer:
		{
			DescriptorBufferInfo buffer = std::get<DescriptorBufferInfo>(descriptor.info);
			Buffer* buf = Buffer::get(buffer.buffer);
			ZE_CHECKF(buf, "Invalid buffer given to descriptor binding {}", descriptor.dst_binding);
			buffer_info.setBuffer(buf->get_buffer());
			buffer_info.setOffset(buffer.offset);
			buffer_info.setRange(buffer.range);
			writes.emplace_back(
				set,
				descriptor.dst_binding,
				0,
				1,
				convert_descriptor_type(descriptor.type),
				nullptr,
				&buffer_info);
			break;
		}
		}
	}

	device.get_device().updateDescriptorSets(writes, {});

}

}