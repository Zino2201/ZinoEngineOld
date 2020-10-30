#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;
class PipelineLayout;

class DescriptorSet
{
public:
	DescriptorSet(Device& in_device, const DescriptorSetCreateInfo& in_create_info);
	~DescriptorSet();

	static DescriptorSet* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE vk::DescriptorSet& get_set() { return set; }
	ZE_FORCEINLINE bool is_valid() const { return !!set; }
private:
	Device& device;
	PipelineLayout* layout;
	uint32_t set_idx;
	vk::DescriptorSet set;
};

}