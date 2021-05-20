#pragma once

#include "EngineCore.h"
#include <cstdarg>
#include <string>
#include <string_view>
#include <chrono>
#include <thread>
#include "flags/Flags.h"
#include "Severity.h"
#include <fmt/format.h>

/**
 * Thread-safe logging system
 */
namespace ze::logger
{

class Sink;

/**
 * A logger message
 */
struct Message
{
	std::chrono::system_clock::time_point time;
	std::thread::id thread_id;
	SeverityFlagBits severity;
	std::string message;

	Message(const std::chrono::system_clock::time_point& in_time,
		const std::thread::id& in_id,
		const SeverityFlagBits& in_severity,
		const std::string& in_message) : time(in_time),
		thread_id(in_id), severity(in_severity), message(in_message) {}
};

/**
 * Print the message to the log
 */
CORE_API void log(SeverityFlagBits severity, const std::string& message);

/**
 * Add a new sink
 */
CORE_API void add_sink(std::unique_ptr<Sink>&& sink);

/**
 * Utils functions for logging
 */

template<typename... Args>
ZE_FORCEINLINE void logf(SeverityFlagBits severity, std::string_view format, Args&&... args)
{
	log(severity, fmt::format(format, std::forward<Args>(args)...));
}

template<typename... Args>
ZE_FORCEINLINE void verbose(const std::string_view& format, Args&&... args)
{
#if ZE_FEATURE(DEVELOPMENT)
	logf(SeverityFlagBits::Verbose, format, std::forward<Args>(args)...);
#endif
}

template<typename... Args>
ZE_FORCEINLINE void info(const std::string_view& format, Args&&... args)
{
	logf(SeverityFlagBits::Info, format, std::forward<Args>(args)...);
}

template<typename... Args>
ZE_FORCEINLINE void warn(const std::string_view& format, Args&&... args)
{
	logf(SeverityFlagBits::Warn, format, std::forward<Args>(args)...);
}

template<typename... Args>
ZE_FORCEINLINE void error(const std::string_view& format, Args&&... args)
{
	logf(SeverityFlagBits::Error, format, std::forward<Args>(args)...);
}

template<typename... Args>
ZE_FORCEINLINE void fatal(const std::string_view& format, Args&&... args)
{
	logf(SeverityFlagBits::Fatal, format, std::forward<Args>(args)...);
}

/**
 * Get severity flag bit as string
 */
inline std::string_view get_severity_as_string(const SeverityFlagBits& flag_bit)
{
	switch(flag_bit)
	{
	case SeverityFlagBits::Verbose:
		return "VERBOSE";
	default:
	case SeverityFlagBits::Info:
		return "INFO";
	case SeverityFlagBits::Warn:
		return "WARN";
	case SeverityFlagBits::Error:
		return "ERROR";
	case SeverityFlagBits::Fatal:
		return "FATAL";
	}
}
}