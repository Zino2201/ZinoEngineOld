#pragma once

#include "Gfx/Backend.h"
#include "Vulkan.h"
#include <optional>

namespace ze::gfx::vulkan
{

class Queue;

/**
 * Structure holding queue family indices 
 */
struct QueueFamilyIndices
{
	std::optional<uint32_t> gfx;

	bool is_complete() const
	{
		return gfx.has_value();
	}
};

class Device
{
	struct AllocatorWrapper
	{
		~AllocatorWrapper()
		{
			vmaDestroyAllocator(allocator);
		}

		VmaAllocator allocator;
	};

public:
	Device(const vk::PhysicalDevice& in_phys_device);
	~Device();

	void create_queues();

	ZE_FORCEINLINE bool is_valid() const { return device && allocator.allocator != VK_NULL_HANDLE; }
	ZE_FORCEINLINE vk::Device& get_device() { return *device; }
	ZE_FORCEINLINE vk::PhysicalDevice& get_physical_device() { return physical_device; }
	ZE_FORCEINLINE ResourceHandle get_gfx_queue() { return gfx_queue; }
	ZE_FORCEINLINE ResourceHandle get_present_queue() { return present_queue; }
	ZE_FORCEINLINE VmaAllocator& get_allocator() { return allocator.allocator; }
	ZE_FORCEINLINE const QueueFamilyIndices& get_queue_family_indices() const { return queue_family_indices; }
private:
	vk::PhysicalDevice physical_device;
	vk::UniqueDevice device;
	AllocatorWrapper allocator;
	ResourceHandle gfx_queue;
	ResourceHandle present_queue;
	QueueFamilyIndices queue_family_indices;
};

}