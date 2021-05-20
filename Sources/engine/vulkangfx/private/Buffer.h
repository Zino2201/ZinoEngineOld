#pragma once

#include "Vulkan.h"
#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class Buffer
{
public:
	Buffer(Device& in_device, const BufferCreateInfo& in_create_info);
	~Buffer();

	Buffer(Buffer&& other) :
		device(other.device),
		buffer(std::exchange(other.buffer, vk::Buffer())),
		allocation(std::exchange(other.allocation, VmaAllocation{})),
		allocation_info(std::exchange(other.allocation_info, VmaAllocationInfo{})),
		create_info(std::exchange(other.create_info, BufferCreateInfo())) {}

	static Buffer* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!buffer; }
	ZE_FORCEINLINE vk::Buffer& get_buffer() { return buffer; }
	ZE_FORCEINLINE VmaAllocation& get_allocation() { return allocation; }
private:
	Device& device;
	vk::Buffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo allocation_info;
	BufferCreateInfo create_info;
	vk::BufferCreateInfo vk_info_elem;
};

}