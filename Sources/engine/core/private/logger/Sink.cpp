#include "logger/Sink.h"
#include "logger/Logger.h"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/chrono.h>
#include "threading/Thread.h"
#include <ctime>
#include <iomanip>

namespace ze::logger
{

std::string Sink::format(const Message& message)
{
	using namespace fmt::literals;
	
	std::time_t time = std::chrono::system_clock::to_time_t(message.time);
	std::tm* localtime = ::localtime(&time);

	std::stringstream time_str;
	time_str << std::put_time(localtime, "%H:%M:%S");

	// TODO: Allow custom formats per sink
	return fmt::format("({time}) [{severity}/{thread_name}] {message}\n",
		"time"_a=time_str.str(),
		"severity"_a=get_severity_as_string(message.severity),
		"thread_name"_a=ze::threading::get_thread_name(message.thread_id),
		"message"_a=message.message.c_str());
}

}