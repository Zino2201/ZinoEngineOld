#pragma once

#include "Vulkan.h"
#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

class CommandPool
{
public:
	CommandPool(Device& in_device, const size_t in_queue_family);

	static CommandPool* get(const ResourceHandle& in_handle);

	void reset();

	ZE_FORCEINLINE bool is_valid() const { return !!pool; }
	ZE_FORCEINLINE vk::CommandPool& get_pool() { return *pool; }
private:
	Device& device;
	vk::UniqueCommandPool pool;
};

class CommandList
{
public:
	CommandList(Device& in_device,
		CommandPool& in_pool,
		const vk::CommandBuffer& buffer);

	CommandList(CommandList&& other) :
		device(other.device),
		pool(other.pool),
		buffer(std::exchange(other.buffer, vk::CommandBuffer())) {}

	static CommandList* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!buffer; }
	ZE_FORCEINLINE vk::CommandPool& get_pool() { return pool.get_pool(); }
	ZE_FORCEINLINE vk::CommandBuffer& get_buffer() { return buffer; }
private:
	Device& device;
	CommandPool& pool;
	vk::CommandBuffer buffer;
};

}