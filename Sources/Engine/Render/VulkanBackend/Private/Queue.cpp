#include "Queue.h"
#include "Device.h"
#include "VulkanBackend.h"
#include "Command.h"
#include "Sync.h"
#include "VulkanUtil.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, Queue> queues;

Queue* Queue::get(const ResourceHandle& in_handle)
{
	auto queue = queues.find(in_handle);

	if(queue != queues.end())
		return &queue->second;
	
	return nullptr;
}

ResourceHandle VulkanBackend::queue_create(const uint32_t& in_family, const uint32_t& in_idx)
{
	ResourceHandle handle;

	Queue queue(*device, in_family, in_idx);
	if(queue.is_valid())
	{
		handle = create_resource_handle(ResourceType::Queue,
			static_cast<VkQueue>(queue.get_queue()), in_family);
		queues.insert({ handle, std::move(queue) });
	}

	return handle;	
}

void VulkanBackend::queue_execute(const ResourceHandle& in_queue, 
	const std::vector<ResourceHandle>& in_command_lists,
	const ResourceHandle& in_fence,
	const std::vector<ResourceHandle>& in_wait_semaphores,
	const std::vector<PipelineStageFlags>& in_wait_stage_flags,
	const std::vector<ResourceHandle>& in_signal_semaphores)
{
	/** Acquire queue */
	Queue* queue = Queue::get(in_queue);
	ZE_CHECKF(queue, "Invalid queue given to queue_execute");

	/** Acquire command lists */
	std::vector<vk::CommandBuffer> buffers;
	buffers.reserve(in_command_lists.size());
	for(const auto& handle : in_command_lists)
	{
		CommandList* list = CommandList::get(handle);
		ZE_CHECKF(list, "Invalid list given to queue_execute");
		buffers.emplace_back(list->get_buffer());
	}

	/** Acquire synchronization primitives */
	Fence* fence = Fence::get(in_fence);
	std::vector<vk::PipelineStageFlags> wait_flags;
	std::vector<vk::Semaphore> wait_semaphores;
	std::vector<vk::Semaphore> signal_semaphores;
	wait_flags.reserve(in_wait_stage_flags.size());
	wait_semaphores.reserve(in_wait_semaphores.size());
	signal_semaphores.reserve(in_signal_semaphores.size());
	
	for(size_t i = 0; i < in_wait_semaphores.size(); ++i)
	{
		Semaphore* semaphore = Semaphore::get(in_wait_semaphores[i]);
		ZE_CHECKF(semaphore, "Invalid wait semaphore given to queue_execute");
		wait_semaphores.emplace_back(semaphore->get_semaphore());
		wait_flags.emplace_back(convert_pipeline_stage_flags(in_wait_stage_flags[i]));
	}

	for(const auto& handle : in_signal_semaphores)
	{
		Semaphore* semaphore = Semaphore::get(handle);
		ZE_CHECKF(semaphore, "Invalid signal semaphore given to queue_execute");
		signal_semaphores.emplace_back(semaphore->get_semaphore());
	}

	queue->get_queue().submit(
		{
			vk::SubmitInfo(
				wait_semaphores,
				wait_flags,
				buffers,
				signal_semaphores)
		},
		fence ? fence->get_fence() : vk::Fence());
}

ResourceHandle VulkanBackend::get_gfx_queue() const
{
	return device->get_gfx_queue();
}

Queue::Queue(Device& in_device, const uint32_t& in_family, const uint32_t& in_idx) 
	: device(in_device), family(in_family), idx(in_idx)
{
	queue = device.get_device().getQueue(family, idx);
}

}