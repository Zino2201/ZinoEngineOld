#include "EngineCore.h"
#include "Logger/Logger.h"
#include <iostream>
#include <mutex>
#include <sstream>
#include "Module/Module.h"
#include <chrono>
#include <filesystem>
#include "Threading/Thread.h"
#include "Logger/Sink.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "App.h"

namespace ze::logger
{

std::mutex logger_mutex;
std::vector<std::unique_ptr<Sink>> sinks;

/**
 * Display a message box for the specified message
 */
void msg_box(const Message& message)
{
#if ZE_PLATFORM(WINDOWS)
	UINT Type = MB_OK;

	switch(message.severity)
	{
	default:
		Type |= MB_ICONINFORMATION;
		break;
	case SeverityFlagBits::Warn:
		Type |= MB_ICONWARNING;
		break;
	case SeverityFlagBits::Error:
	case SeverityFlagBits::Fatal:
		Type |= MB_ICONERROR;
		break;
	}

	MessageBoxA(nullptr, message.message.c_str(), 
		"ZinoEngine Fatal Error", Type);
#endif
}

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
		msg_box(message);
#if ZE_DEBUG
		ZE_DEBUGBREAK();
#endif
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