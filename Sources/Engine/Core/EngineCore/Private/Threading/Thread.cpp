#include "Threading/Thread.h"
#include "Logger.h"
#include <thread>
#include <mutex>
#include <robin_hood.h>

namespace ZE::Threading
{

robin_hood::unordered_map<std::thread::id, std::string> ThreadNames;
std::mutex ThreadNamesMutex;

void SetThreadName(const std::string_view& InStr)
{
	std::lock_guard<std::mutex> Guard(ThreadNamesMutex);
	ThreadNames[std::this_thread::get_id()] = InStr;
}

std::string GetThreadName()
{
	std::lock_guard<std::mutex> Guard(ThreadNamesMutex);
	return ThreadNames[std::this_thread::get_id()];
}

}