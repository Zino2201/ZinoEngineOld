#include "Command.h"
#include "Device.h"
#include "VulkanBackend.h"
#include "VulkanUtil.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> pools;
robin_hood::unordered_set<ResourceHandle> lists;
#endif

ResourceHandle VulkanBackend::command_pool_create(CommandPoolType in_type)
{
	size_t queue_family = 0;
	// TODO: support multiple types
	ZE_CHECK(in_type == CommandPoolType::Gfx);

	ResourceHandle handle = create_resource<CommandPool>(ResourceType::CommandPool, 
		*device, queue_family);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	pools.insert(handle);
#endif

	return handle;
}

void VulkanBackend::command_pool_destroy(const ResourceHandle& in_handle)
{
	delete_resource<CommandPool>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	pools.erase(in_handle);
#endif
}

void VulkanBackend::command_pool_reset(const ResourceHandle& in_pool)
{
	if(CommandPool* pool = CommandPool::get(in_pool))
	{
		pool->reset();
	}
}

std::vector<ResourceHandle> VulkanBackend::command_pool_allocate(const ResourceHandle& in_pool,
	const size_t in_count)
{
	CommandPool* pool = CommandPool::get(in_pool);
	ZE_CHECKF(pool, "Invalid pool given to command_pool_allocate");

	std::vector<ResourceHandle> handles;
	handles.reserve(in_count);

	std::vector<vk::CommandBuffer> buffers = device->get_device().allocateCommandBuffers(
		vk::CommandBufferAllocateInfo(
			pool->get_pool(),
			vk::CommandBufferLevel::ePrimary,
			in_count));

	for(const auto& buffer : buffers)
	{
		ResourceHandle handle = create_resource<CommandList>(ResourceType::CommandList,
			*device, *pool, buffer);
		handles.emplace_back(handle);
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
		lists.insert(handle);
#endif
	}

	return handles;
}

Result VulkanBackend::command_list_begin(const ResourceHandle& in_command_list)
{
	CommandList* list = CommandList::get(in_command_list);
	ZE_CHECKF(list, "Invalid list was given to command_list_begin");
	
	return convert_vk_result(
		list->get_buffer().begin(vk::CommandBufferBeginInfo(
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit)));
}

Result VulkanBackend::command_list_end(const ResourceHandle& in_command_list)
{
	CommandList* list = CommandList::get(in_command_list);
	ZE_CHECKF(list, "Invalid list was given to command_list_end");
	
	return convert_vk_result(
		list->get_buffer().end());
}

CommandPool::CommandPool(Device& in_device, const size_t in_queue_family)
	: device(in_device)
{
	auto [result, handle] = device.get_device().createCommandPoolUnique(
		vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlags(),
			in_queue_family));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create command pool: {}",
			vk::to_string(result));

	pool = std::move(handle);
}

void CommandPool::reset()
{
	device.get_device().resetCommandPool(*pool, vk::CommandPoolResetFlags());
}

CommandPool* CommandPool::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto pool = pools.find(in_handle);
	ZE_CHECKF(pool != pools.end(), "Invalid command pool");
#endif
	
	return get_resource<CommandPool>(in_handle);
}

/** Command list */
CommandList::CommandList(Device& in_device,
	CommandPool& in_pool, const vk::CommandBuffer& in_buffer) : device(in_device), pool(in_pool)
{
	buffer = in_buffer;
}

CommandList* CommandList::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto list = lists.find(in_handle);
	ZE_CHECKF(list != lists.end(), "Invalid command list");
#endif

	return get_resource<CommandList>(in_handle);
}

}