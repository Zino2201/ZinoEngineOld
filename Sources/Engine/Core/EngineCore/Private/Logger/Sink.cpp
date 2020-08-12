#include "Logger/Sink.h"
#include "Logger/Logger.h"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/chrono.h>
#include "Threading/Thread.h"
#include <ctime>
#include <iomanip>

namespace ZE::Logger
{

std::string CSink::Format(const SMessage& InMessage)
{
	using namespace fmt::literals;
	
	std::time_t Time = std::chrono::system_clock::to_time_t(InMessage.Time);
	std::tm* LocalTime = localtime(&Time);

	std::stringstream TimeStr;
	TimeStr << std::put_time(LocalTime, "%H:%M:%S");

	// TODO: Allow custom formats per sink
	return fmt::format("({time}) [{severity}/{thread_name}] {message}\n",
		"time"_a=TimeStr.str(),
		"severity"_a=GetSeverityAsString(InMessage.Severity),
		"thread_name"_a=ZE::Threading::GetThreadName(InMessage.ThreadId),
		"message"_a=InMessage.Message.c_str());
}

}