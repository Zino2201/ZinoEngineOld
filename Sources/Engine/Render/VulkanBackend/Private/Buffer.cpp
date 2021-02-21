#include "Buffer.h"
#include "VulkanUtil.h"
#include "Device.h"
#include "VulkanBackend.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> buffers;
#endif

std::pair<Result, void*> VulkanBackend::buffer_map(const ResourceHandle& in_buffer)
{
	Buffer* buffer = Buffer::get(in_buffer);
	ZE_CHECKF(buffer, "Invalid buffer given to buffer_map");

	void* data;
	vk::Result result = static_cast<vk::Result>(vmaMapMemory(device->get_allocator(),
		buffer->get_allocation(),
		&data));

	return { convert_vk_result(result), data };
}

void VulkanBackend::buffer_unmap(const ResourceHandle& in_buffer)
{
	Buffer* buffer = Buffer::get(in_buffer);
	ZE_CHECKF(buffer, "Invalid buffer given to buffer_unmap");

	vmaUnmapMemory(device->get_allocator(), buffer->get_allocation());
}

ResourceHandle VulkanBackend::buffer_create(const BufferCreateInfo& in_create_info) 
{
	ResourceHandle handle = create_resource<Buffer>(ResourceType::Buffer, 
		*device, in_create_info);
	buffers.insert(handle);
	return handle;
}

Buffer::Buffer(Device& in_device, const BufferCreateInfo& in_create_info)
	: device(in_device), create_info(in_create_info)
{
	vk::BufferUsageFlags usage_flags;
	
	if(in_create_info.usage & BufferUsageFlagBits::TransferSrc)
		usage_flags |= vk::BufferUsageFlagBits::eTransferSrc;

	if (in_create_info.usage & BufferUsageFlagBits::TransferDst)
		usage_flags |= vk::BufferUsageFlagBits::eTransferDst;

	if(in_create_info.usage & BufferUsageFlagBits::VertexBuffer)
		usage_flags |= vk::BufferUsageFlagBits::eVertexBuffer;

	if(in_create_info.usage & BufferUsageFlagBits::IndexBuffer)
		usage_flags |= vk::BufferUsageFlagBits::eIndexBuffer;

	if(in_create_info.usage & BufferUsageFlagBits::UniformBuffer)
		usage_flags |= vk::BufferUsageFlagBits::eUniformBuffer;

	if(in_create_info.usage & BufferUsageFlagBits::StorageBuffer)
		usage_flags |= vk::BufferUsageFlagBits::eStorageBuffer;

	vk::BufferCreateInfo buffer_create_info(
		vk::BufferCreateFlags(),
		in_create_info.size,
		usage_flags,
		vk::SharingMode::eExclusive);

	VmaAllocationCreateInfo alloc_create_info = {};
	alloc_create_info.flags = 0;
	alloc_create_info.usage = convert_memory_usage(in_create_info.mem_usage);

	vk::Result result = static_cast<vk::Result>(vmaCreateBuffer(in_device.get_allocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&buffer_create_info),
		&alloc_create_info,
		reinterpret_cast<VkBuffer*>(&buffer),
		&allocation,
		&allocation_info));
	if (result != vk::Result::eSuccess)
		ze::logger::error("Failed to create Vulkan buffer: {}",
			vk::to_string(result).c_str());
}

Buffer::~Buffer()
{
	if(buffer)
		vmaDestroyBuffer(device.get_allocator(), buffer, allocation);
}

Buffer* Buffer::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto buffer = buffers.find(in_handle);
	ZE_CHECKF(buffer != buffers.end(), "Invalid buffer");
#endif
	
	return reinterpret_cast<Buffer*>(reinterpret_cast<uintptr_t>(in_handle.handle));
}

void VulkanBackend::buffer_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Buffer>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	buffers.erase(in_handle);
#endif
}

}