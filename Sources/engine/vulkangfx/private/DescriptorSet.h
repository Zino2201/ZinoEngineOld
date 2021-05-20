#pragma once

#include "Vulkan.h"
#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class DescriptorSet
{
public:
	DescriptorSet(Device& in_device, const vk::DescriptorSet& in_set);
	~DescriptorSet();

	DescriptorSet(DescriptorSet&& other) :
		device(other.device),
		set(std::exchange(other.set, vk::DescriptorSet())) {}

	/*
	 * Update the descriptor set with the specified descriptors
	 * \return Hash of descriptors
	 */
	void update(const uint64_t& in_hash, const std::vector<Descriptor>& in_descriptors);

	static DescriptorSet* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE vk::DescriptorSet& get_set() { return set; }
	ZE_FORCEINLINE uint64_t get_hash() { return hash; }
	ZE_FORCEINLINE bool is_valid() const { return !!set; }
private:
	Device& device;
	vk::DescriptorSet set;
	uint64_t hash;
};

}