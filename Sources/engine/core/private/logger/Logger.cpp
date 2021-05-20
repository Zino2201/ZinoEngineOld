#include "EngineCore.h"
#include "logger/Logger.h"
#include <iostream>
#include <mutex>
#include <sstream>
#include "module/Module.h"
#include <chrono>
#include <filesystem>
#include "threading/Thread.h"
#include "logger/Sink.h"
#include "App.h"
#include "MessageBox.h"

namespace ze::logger
{

std::mutex logger_mutex;
std::vector<std::unique_ptr<Sink>> sinks;

void log(SeverityFlagBits severity, const std::string& str)
{
	std::lock_guard<std::mutex> guard(logger_mutex);

	Message message(
		std::chrono::system_clock::now(),
		std::this_thread::get_id(),
		severity,
		std::move(str));

	/**
	 * Call sinks
	 */
	for(const auto& sink : sinks)
	{
		if(sink->get_severity_flags() & severity)
			sink->log(message);
	}

	if(severity == SeverityFlagBits::Fatal)
	{
		message_box("ZinoEngine Fatal Error", message.message.c_str(),
			MessageBoxButtonFlagBits::Ok, MessageBoxIcon::Critical);
		app::exit(-1);
	}
}

/**
 * Sink manipulation
 */
void add_sink(std::unique_ptr<Sink>&& sink)
{
	/**
	 * Scope so that we can print a verbose message without making a infinite mutex loop
	 */
	{
		std::lock_guard<std::mutex> guard(logger_mutex);
		sinks.push_back(std::move(sink));
	}

	verbose("Added sink {}", sinks.back()->get_name());
}

}