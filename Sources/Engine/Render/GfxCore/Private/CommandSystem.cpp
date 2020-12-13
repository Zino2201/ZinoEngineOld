#include "Gfx/CommandSystem.h"
#include <queue>
#include <mutex>

namespace ze::gfx::hlcs
{

struct CommandPool;

std::vector<CommandPool*> pools;

/**
 * Command pool wrapper that register the pool to a global array
 */	
struct CommandPool
{
	gfx::UniqueCommandPool pool;
	std::vector<CommandList> lists;
	std::queue<CommandList*> recycled_transient_lists;

	CommandPool()
	{
		pools.emplace_back(this);
	}

	~CommandPool()
	{
		for(size_t i = 0; i < pools.size(); ++i)
		{
			if(pools[i] == this)
			{
				pools.erase(pools.begin() + i);
				break;
			}
		}
	}

	CommandList& allocate_list(const bool in_is_transient)
	{
		if(in_is_transient && !recycled_transient_lists.empty())
		{
			CommandList& list = *recycled_transient_lists.front();
			recycled_transient_lists.pop();
			list.should_execute = true;
			return list;
		}

		lists.emplace_back(lists.size(), in_is_transient);
		lists.back().handle = RenderBackend::get().command_pool_allocate(*pool, 1).front();
		return lists.back();
	}

	void free(CommandList& in_list)
	{
		/** Free buffers */
		for(const auto& buffer : in_list.owned_buffers)
		{
			RenderBackend::get().buffer_destroy(buffer);
		}

		in_list.owned_buffers.clear();

		if(in_list.is_transient())
		{
			in_list.should_execute = false;
			recycled_transient_lists.push(&in_list);
		}
	}
	
	/**
	 * Reset the pool
	 * Will recycle any transient command lists to a queue and will free any attached resources to command lists
	 */
	void reset()
	{
		if(pool)
			RenderBackend::get().command_pool_reset(*pool);

		for(auto& list : lists)
		{
			list.on_executed.execute();
			list.on_executed.clear();
			free(list);
		}
	}

	void destroy()
	{
		pool.reset();
	}
};

thread_local CommandPool pool;
gfx::UniqueFence fence;
std::vector<ResourceHandle> resources_to_free;
std::mutex resource_to_free_mutex;
bool has_executed_commands = false;

void initialize()
{
	fence = RenderBackend::get().fence_create();

	for(const auto& pool : pools)
	{
		pool->pool = RenderBackend::get().command_pool_create(gfx::CommandPoolType::Gfx);
	}
}

void begin_new_frame()
{
	if(has_executed_commands)
	{
		RenderBackend::get().fence_wait_for({ *fence });
		RenderBackend::get().fence_reset({ *fence });

		for(const auto& pool : pools)
		{
			 pool->reset();
		}

		for(const auto& resource : resources_to_free)
		{
			switch(resource.type)
			{
			case ResourceType::Buffer:
				RenderBackend::get().buffer_destroy(resource);
				break;
			case ResourceType::Texture:
				RenderBackend::get().texture_destroy(resource);
				break;
			case ResourceType::TextureView:
				RenderBackend::get().texture_view_destroy(resource);
				break;
			default:
				ZE_CHECK(false);
				break;
			}
		}

		has_executed_commands = false;
	}
}

void destroy()
{
	for(const auto& pool : pools)
		pool->destroy();

	fence.reset();
}

void queue_destroy(const gfx::ResourceHandle& in_handle)
{
	std::lock_guard<std::mutex> guard(resource_to_free_mutex);
	resources_to_free.emplace_back(in_handle);
}

CommandList& allocate_cmd_list(bool in_is_transient)
{
	return pool.allocate_list(in_is_transient);
}

void submit_all_lists()
{
	std::vector<ResourceHandle> command_lists;

	for(const auto& pool : pools)
	{
		 for(const auto& list : pool->lists)
		 {
			 if(list.should_execute_list())
			 {
				command_lists.emplace_back(list.get_handle());
			 }
		 }
	}

	if(!command_lists.empty())
	{
		RenderBackend::get().queue_execute(
			RenderBackend::get().get_gfx_queue(),
			command_lists,
			*fence);
		has_executed_commands = true;
	}
}

void submit(CommandList& in_list, const ResourceHandle& in_fence_to_signal)
{
	if(!in_list.should_execute_list())
		return;

	RenderBackend::get().queue_execute(
		RenderBackend::get().get_gfx_queue(),
		{ in_list.get_handle() },
		in_fence_to_signal);

	in_list.mark_as_executed();
}

}