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

	struct SetEntry
	{
		static constexpr uint8_t max_set_lifetime = 10;

		ResourceHandle set;
		uint8_t lifetime;

		SetEntry(const ResourceHandle& in_set) : set(in_set), lifetime(0) {}

		ResourceHandle get_set()
		{
			lifetime = 0;
			return set;
		}
	};
public:
	PipelineLayout(Device& in_device, const PipelineLayoutCreateInfo& in_create_info);

	void new_frame();
	static void update_layouts();

	/**
	 * Will allocate a set matching this pipeline layout
	 * This function may recycle an already allocated descriptor set
	 */
	ResourceHandle allocate_set(const uint32_t in_set, const std::vector<Descriptor>& in_descriptors);
	void free_set(const uint32_t in_set_idx, const ResourceHandle& in_set);

	static PipelineLayout* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!layout && !descriptor_pools.empty(); }
	ZE_FORCEINLINE vk::PipelineLayout& get_layout() { return *layout; }
private:
	void allocate_pool();	
	ResourceHandle allocate_set_internal(PoolEntry& in_pool, const uint32_t in_set);
private:
	Device& device;
	vk::UniquePipelineLayout layout;
	std::vector<vk::UniqueDescriptorSetLayout> set_layouts;

	/** Desc set mgmt */
	robin_hood::unordered_map<uint32_t, robin_hood::unordered_map<uint64_t, SetEntry>> descriptor_sets;
	robin_hood::unordered_map<uint32_t, std::queue<ResourceHandle>> free_descriptor_sets;
	std::vector<PoolEntry> descriptor_pools;
	std::vector<vk::DescriptorPoolSize> descriptor_pool_sizes;
};

}