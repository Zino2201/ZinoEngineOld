#include "PipelineLayout.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include "Device.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

static constexpr uint32_t max_descriptor_sets_per_pool = 32;
static constexpr uint32_t default_descriptor_count_per_type = 32;

robin_hood::unordered_map<ResourceHandle, PipelineLayout> pipeline_layouts;
vk::Result last_pipeline_layout_result;

std::pair<Result, ResourceHandle> VulkanBackend::pipeline_layout_create(const PipelineLayoutCreateInfo& in_create_info)
{
	ResourceHandle handle;

	PipelineLayout layout(*device, in_create_info);
	if(layout.is_valid())
	{
		handle = create_resource_handle(ResourceType::PipelineLayout, 
			static_cast<VkPipelineLayout>(layout.get_layout()), in_create_info);
		pipeline_layouts.insert({ handle, std::move(layout) });
	}

	return { convert_vk_result(last_pipeline_layout_result), handle };
}

void VulkanBackend::pipeline_layout_destroy(const ResourceHandle& in_handle)
{
	pipeline_layouts.erase(in_handle);
}

PipelineLayout::PipelineLayout(Device& in_device, 
	const PipelineLayoutCreateInfo& in_create_info) 
	: device(in_device)
{
	std::vector<vk::DescriptorSetLayout> set_layouts_non_unique;

	/** Create set layouts */
	for(const auto& layout : in_create_info.set_layouts)
	{
		uint64_t hash_desc_types = 0;

		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		bindings.reserve(layout.bindings.size());
		for(const auto& binding : layout.bindings)
		{
			ze::hash_combine(hash_desc_types, binding.descriptor_type);
			bindings.emplace_back(binding.binding,
				convert_descriptor_type(binding.descriptor_type),
				binding.count,
				convert_shader_stage_flags(binding.stage_flags));
		}

		descriptor_hashes.emplace_back(hash_desc_types);

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
	auto layout = pipeline_layouts.find(in_handle);

	if(layout != pipeline_layouts.end())
		return &layout->second;

	return nullptr;
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

vk::DescriptorSet PipelineLayout::allocate_set(const uint32_t in_set)
{
	/** Search for set to recycle */
	auto& queue = free_descriptor_sets[in_set];
	if(!queue.empty())
	{
		vk::DescriptorSet set = queue.front();
		queue.pop();
		return set;
	}

	/** Allocate a new set */
	auto& pool = descriptor_pools.back();
	if(pool.allocations < max_descriptor_sets_per_pool)
	{
		return allocate_set_internal(pool, in_set);
	}
	else
	{
		allocate_pool();
		return allocate_set_internal(descriptor_pools.back(), in_set);
	}
}

void PipelineLayout::free_set(const uint32_t in_set_idx, const vk::DescriptorSet& in_set)
{
	free_descriptor_sets[in_set_idx].push(in_set);
}

vk::DescriptorSet PipelineLayout::allocate_set_internal(PoolEntry& in_pool, const uint32_t in_set)
{
	auto [result, handle] = device.get_device().allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo(
			*in_pool.pool,
			1,
			&*set_layouts[in_set]));
	ZE_CHECKF(result == vk::Result::eSuccess, "Failed to allocate descriptor set");
	if (result != vk::Result::eSuccess)
		ze::logger::error("Failed to allocate descriptor set: {}", vk::to_string(result));

	return handle.front();
}

uint32_t PipelineLayout::get_set_from_descriptors_hash(const uint64_t in_hash)
{
	for(size_t i = 0; i < descriptor_hashes.size(); ++i)
	{
		if(descriptor_hashes[i] == in_hash)
			return i;
	}

	return -1;
}

}