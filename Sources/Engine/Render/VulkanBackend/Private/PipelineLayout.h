#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"
#include <queue>
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

class Device;

class PipelineLayout
{
	struct PoolEntry
	{
		uint32_t allocations;
		vk::UniqueDescriptorPool pool;
		
		PoolEntry(vk::UniqueDescriptorPool&& in_pool) : allocations(0), pool(std::move(in_pool)) {}
	};

public:
	PipelineLayout(Device& in_device, const PipelineLayoutCreateInfo& in_create_info);

	/**
	 * Will allocate a set matching this pipeline layout
	 * This function may recycle an already allocated descriptor set
	 */
	vk::DescriptorSet allocate_set(const uint32_t in_set);
	void free_set(const uint32_t in_set_idx, const vk::DescriptorSet& in_set);

	static PipelineLayout* get(const ResourceHandle& in_handle);

	uint32_t get_set_from_descriptors_hash(const uint64_t in_hash);
	ZE_FORCEINLINE bool is_valid() const { return !!layout && !descriptor_pools.empty(); }
	ZE_FORCEINLINE vk::PipelineLayout& get_layout() { return *layout; }
private:
	void allocate_pool();	
	vk::DescriptorSet allocate_set_internal(PoolEntry& in_pool, const uint32_t in_set);
private:
	Device& device;
	vk::UniquePipelineLayout layout;
	std::vector<vk::UniqueDescriptorSetLayout> set_layouts;
	std::vector<uint64_t> descriptor_hashes;

	/** Desc set mgmt */
	std::unordered_map<uint32_t, std::queue<vk::DescriptorSet>> free_descriptor_sets;
	std::vector<PoolEntry> descriptor_pools;
	std::vector<vk::DescriptorPoolSize> descriptor_pool_sizes;
};

}