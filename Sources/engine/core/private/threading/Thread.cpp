#include "threading/Thread.h"
#include "logger/Logger.h"
#include <thread>
#include <mutex>
#include <robin_hood.h>

namespace ze::threading
{

robin_hood::unordered_map<std::thread::id, std::string> thread_names;
std::mutex thread_names_mutex;

void set_thread_name(const std::string_view& name)
{
	std::lock_guard<std::mutex> guard(thread_names_mutex);
	thread_names[std::this_thread::get_id()] = name;
}

std::string get_thread_name()
{
	std::lock_guard<std::mutex> guard(thread_names_mutex);
	return thread_names[std::this_thread::get_id()];
}

std::string get_thread_name(const std::thread::id& id)
{
	std::lock_guard<std::mutex> guard(thread_names_mutex);
	return thread_names[id];
}

}