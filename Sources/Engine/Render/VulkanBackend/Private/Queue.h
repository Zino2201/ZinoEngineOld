#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class Queue
{
public:
	Queue(Device& in_device, const uint32_t& in_family,
		const uint32_t& in_idx);

	static Queue* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE const uint32_t& get_family() const { return family; }
	ZE_FORCEINLINE vk::Queue& get_queue() { return queue; }
	ZE_FORCEINLINE bool is_valid() const { return queue; }
	
	bool operator!=(const Queue& other) const
	{
		return queue != other.queue;
	}
private:
	Device& device;
	vk::Queue queue;
	uint32_t family;
	uint32_t idx;
};

}