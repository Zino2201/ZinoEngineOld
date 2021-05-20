#include "Device.h"
#include <set>
#include "Queue.h"
#include "VulkanBackend.h"

namespace ze::gfx::vulkan
{

Device::Device(const vk::PhysicalDevice& in_phys_device) :
	physical_device(in_phys_device)
{
	/** Get queue family indices */
	{
		std::vector<vk::QueueFamilyProperties> queue_families = physical_device.getQueueFamilyProperties();

		/** Present queue use gfx queue by default */
		int i = 0;
		for (const vk::QueueFamilyProperties& queue_family : queue_families)
		{
			if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				queue_family_indices.gfx = i;
			}

			if (queue_family_indices.is_complete())
				break;

			i++;
		}
	}

	/** Queue create infos */
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
	{
		std::set<uint32_t> unique_queue_families =
		{
			queue_family_indices.gfx.value()
		};

		float priority = 1.f;
		for (uint32_t family : unique_queue_families)
		{
			queue_create_infos.emplace_back(
				vk::DeviceQueueCreateFlags(),
				family,
				1,
				&priority);
		}
	}

	vk::DeviceCreateInfo device_create_infos(
		vk::DeviceCreateFlags(),
		static_cast<uint32_t>(queue_create_infos.size()),
		queue_create_infos.data(),
		enable_validation_layers ? static_cast<uint32_t>(validation_layers.size()) : 0,
		enable_validation_layers ? validation_layers.data() : 0,
		static_cast<uint32_t>(required_device_extensions.size()),
		required_device_extensions.data());
	auto [result, handle] = physical_device.createDeviceUnique(device_create_infos);
	if (result != vk::Result::eSuccess)
		ze::logger::error("Failed to create logical device: {}", vk::to_string(result));
	device = std::move(handle);
	
	/** Create allocator */
	{
		VmaAllocatorCreateInfo alloc_create_info = {};
		alloc_create_info.device = *device;
		alloc_create_info.physicalDevice = physical_device;
		vk::Result result = static_cast<vk::Result>(vmaCreateAllocator(&alloc_create_info, &allocator.allocator));
		if(result != vk::Result::eSuccess) 
			ze::logger::error("Failed to create allocator: {}", vk::to_string(result));
	}
}

Device::~Device() = default;

void Device::create_queues()
{
	/** Create queues */
	gfx_queue = get_backend().queue_create(queue_family_indices.gfx.value(), 0);	
	present_queue = gfx_queue;
}

}