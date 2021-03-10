#include "PipelineLayout.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include "Device.h"
#include "DescriptorSet.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

static constexpr uint32_t max_descriptor_sets_per_pool = 32;
static constexpr uint32_t default_descriptor_count_per_type = 32;

robin_hood::unordered_set<ResourceHandle> pipeline_layouts;
vk::Result last_pipeline_layout_result;

void PipelineLayout::update_layouts()
{
	for(auto& handle : pipeline_layouts)
	{
		PipelineLayout* layout = PipelineLayout::get(handle);
		layout->new_frame();
	}
}

std::pair<Result, ResourceHandle> VulkanBackend::pipeline_layout_create(const PipelineLayoutCreateInfo& in_create_info)
{
	ResourceHandle handle = create_resource<PipelineLayout>(*device, in_create_info);

	pipeline_layouts.insert(handle);

	return { convert_vk_result(last_pipeline_layout_result), handle };
}

void VulkanBackend::pipeline_layout_destroy(const ResourceHandle& in_handle)
{
	delete_resource<PipelineLayout>(in_handle);

	pipeline_layouts.erase(in_handle);
}

ResourceHandle VulkanBackend::pipeline_layout_allocate_descriptor_set(const ResourceHandle& in_pipeline_layout,
	const uint32_t in_set,
	const std::vector<Descriptor>& descriptors)
{
	PipelineLayout* layout = PipelineLayout::get(in_pipeline_layout);
	ZE_CHECKF(layout, "Invalid pipeline layout given to pipeline_layout_allocate_descriptor_set");

	return layout->allocate_set(in_set, descriptors);
}

PipelineLayout::PipelineLayout(Device& in_device, 
	const PipelineLayoutCreateInfo& in_create_info) 
	: device(in_device)
{
	std::vector<vk::DescriptorSetLayout> set_layouts_non_unique;

	/** Create set layouts */
	for(const auto& layout : in_create_info.set_layouts)
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		bindings.reserve(layout.bindings.size());
		for(const auto& binding : layout.bindings)
		{
			bindings.emplace_back(binding.binding,
				convert_descriptor_type(binding.descriptor_type),
				binding.count,
				convert_shader_stage_flags(binding.stage_flags));
		}

		auto [result, handle] = device.get_device().createDescriptorSetLayoutUnique(
			vk::DescriptorSetLayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				bindings));
		if(!handle)
		{
			ze::logger::error("Failed to create descriptor set layout: {}", vk::to_string(result));
			return;
		}

		set_layouts_non_unique.push_back(*handle);
		set_layouts.emplace_back(std::move(handle));
	}

	/** Push constants */
	std::vector<vk::PushConstantRange> push_constants;
	push_constants.reserve(in_create_info.push_constant_ranges.size());
	for(const auto& push_constant : in_create_info.push_constant_ranges)
	{
		push_constants.emplace_back(
			convert_shader_stage_flags(push_constant.stage_flags),
			push_constant.offset,
			push_constant.size);
	}

	auto [result, handle] = device.get_device().createPipelineLayoutUnique(
		vk::PipelineLayoutCreateInfo(
			vk::PipelineLayoutCreateFlags(),
			set_layouts_non_unique,
			push_constants));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create pipeline layout: {}", vk::to_string(result));

	layout = std::move(handle);
	last_pipeline_layout_result = result;

	descriptor_pool_sizes = 
	{
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, default_descriptor_count_per_type),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, default_descriptor_count_per_type),
		vk::DescriptorPoolSize(vk::DescriptorType::eSampler, default_descriptor_count_per_type),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, default_descriptor_count_per_type),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic, default_descriptor_count_per_type),
		vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment, default_descriptor_count_per_type),
	};

	if(layout)
		allocate_pool();
}

PipelineLayout* PipelineLayout::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto layout = pipeline_layouts.find(in_handle);
	ZE_CHECKF(layout != pipeline_layouts.end(), "Invalid pipeline layout");
#endif
	return get_resource<PipelineLayout>(in_handle);
}

void PipelineLayout::new_frame()
{
	std::vector<std::pair<uint32_t, uint64_t>> sets_to_recycle;
	sets_to_recycle.reserve(16);

	for(auto& [set, sets] : descriptor_sets)
	{
		for(auto& [hash, entry] : sets)
		{
			entry.lifetime++;
			if(entry.lifetime >= SetEntry::max_set_lifetime)
				sets_to_recycle.emplace_back(set, hash);
		}
	}

	for(const auto& [set, hash] : sets_to_recycle)
	{
		descriptor_sets[set].erase(hash);
	}
}

void PipelineLayout::allocate_pool()
{
	auto [result, handle] = device.get_device().createDescriptorPoolUnique(
		vk::DescriptorPoolCreateInfo(
			vk::DescriptorPoolCreateFlags(),
			max_descriptor_sets_per_pool,
			descriptor_pool_sizes));
	ZE_CHECKF(result == vk::Result::eSuccess, "Failed to create descriptor pool");
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create descriptor pool: {}", vk::to_string(result));

	descriptor_pools.emplace_back(std::move(handle));
}

ResourceHandle PipelineLayout::allocate_set(const uint32_t in_set, const std::vector<Descriptor>& in_descriptors)
{
	ResourceHandle handle;

	uint64_t hash = 0;
	for(const auto& desc : in_descriptors)
		hash_combine(hash, desc);

	auto it = descriptor_sets[in_set].find(hash);
	if(it != descriptor_sets[in_set].end())
		return it->second.get_set();

	/** Search for set to recycle */
	auto& queue = free_descriptor_sets[in_set];
	if(!queue.empty())
	{
		handle = queue.front();
		queue.pop();
	}
	else
	{
		/** Allocate a new set */
		auto& pool = descriptor_pools.back();
		if(pool.allocations < max_descriptor_sets_per_pool)
		{
			handle = allocate_set_internal(pool, in_set);
		}
		else
		{
			allocate_pool();
			handle = allocate_set_internal(descriptor_pools.back(), in_set);
		}
	}

	DescriptorSet* set = DescriptorSet::get(handle);
	ZE_CHECK(set);

	set->update(hash, in_descriptors);
	descriptor_sets[in_set].insert_or_assign(hash, SetEntry(handle));
	return handle;
}

void PipelineLayout::free_set(const uint32_t in_set_idx, const ResourceHandle& in_set)
{
	DescriptorSet* set = DescriptorSet::get(in_set);
	ZE_CHECK(set);
	descriptor_sets[in_set_idx].erase(set->get_hash());
	free_descriptor_sets[in_set_idx].push(in_set);
}

ResourceHandle PipelineLayout::allocate_set_internal(PoolEntry& in_pool, const uint32_t in_set)
{
	auto [result, handle] = device.get_device().allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo(
			*in_pool.pool,
			1,
			&*set_layouts[in_set]));
	ZE_CHECKF(result == vk::Result::eSuccess, "Failed to allocate descriptor set: {}", vk::to_string(result));
	if (result != vk::Result::eSuccess)
		ze::logger::error("Failed to allocate descriptor set: {}", vk::to_string(result));

	in_pool.allocations++;

	return get_backend().descriptor_set_create(handle.front()).second;
}

}