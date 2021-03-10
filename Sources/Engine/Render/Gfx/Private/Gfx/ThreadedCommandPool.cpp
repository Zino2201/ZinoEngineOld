#include "Gfx/Gfx.h"

namespace ze::gfx
{

ThreadedCommandPool::Pool::Pool() : free_command_list(0)
{
	handle = Backend::get().command_pool_create();
}

ThreadedCommandPool::Pool::~Pool()
{
	Backend::get().command_pool_destroy(handle);
}

void ThreadedCommandPool::Pool::new_frame()
{
	Backend::get().command_pool_reset(handle);
	free_command_list = 0;
	for(auto& list : command_lists)
		list->reset();
}

void ThreadedCommandPool::Pool::trim()
{
	for(const auto& list : command_lists)
		Backend::get().command_pool_free(handle, { list->get_backend_handle() });
	command_lists.clear();

	if(Backend::get().support_feature(BackendFeatureFlagBits::CommandPoolTrimming))
		Backend::get().command_pool_trim(handle);
}

CommandList* ThreadedCommandPool::Pool::allocate_cmd_list(CommandListType type)
{
	if(free_command_list < command_lists.size())
	{
		return command_lists[free_command_list++].get();
	}
	else
	{
		command_lists.emplace_back(std::make_unique<CommandList>(type, Backend::get().command_pool_allocate(handle, 1)[0]));
		free_command_list++;
		return command_lists.back().get();
	}
}

ThreadedCommandPool::ThreadedCommandPool(CommandListType in_type) :
	type(in_type) {}

void ThreadedCommandPool::new_frame()
{
	for(auto& [thread, pool] : command_pools)
		pool.new_frame();
}

CommandList* ThreadedCommandPool::allocate_cmd_list()
{
	return get_pool().allocate_cmd_list(type);
}

void ThreadedCommandPool::clear()
{
	command_pools.clear();
}

void ThreadedCommandPool::trim()
{
	for(auto& [thread, pool] : command_pools)
		pool.trim();
}

}