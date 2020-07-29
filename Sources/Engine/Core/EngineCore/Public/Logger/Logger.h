#pragma once

#include "EngineCore.h"
#include <cstdarg>
#include <string>
#include <string_view>
#include <chrono>
#include <thread>
#include "Flags/Flags.h"
#include "Severity.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

/**
 * Thread-safe logging system
 */
namespace ZE::Logger
{

class CSink;

/**
 * A logger message
 */
struct SMessage
{
	std::chrono::system_clock::time_point Time;
	std::thread::id ThreadId;
	ESeverityFlagBits Severity;
	std::string Message;

	SMessage(const std::chrono::system_clock::time_point& InTime,
		const std::thread::id& InId,
		const ESeverityFlagBits& InSeverity,
		const std::string& InMessage) : Time(InTime),
		ThreadId(InId), Severity(InSeverity), Message(InMessage) {}
};

/**
 * Print the message to the log
 */
ENGINECORE_API void Log(ESeverityFlagBits InSeverity, const std::string& InMessage);

/**
 * Add a new sink
 */
ENGINECORE_API void AddSink(std::unique_ptr<CSink>&& InSink);

/**
 * Utils functions for logging
 */

template<typename... Args>
inline void Logf(ESeverityFlagBits InSeverity, std::string_view InFormat, Args&&... InArgs)
{
	Log(InSeverity, fmt::format(InFormat, std::forward<Args>(InArgs)...));
}

template<typename... Args>
inline void Verbose(const std::string_view& InFormat, Args&&... InArgs)
{
#ifdef _DEBUG
	Logf(ESeverityFlagBits::Verbose, InFormat, std::forward<Args>(InArgs)...);
#endif
}

template<typename... Args>
inline void Info(const std::string_view& InFormat, Args&&... InArgs)
{
	Logf(ESeverityFlagBits::Info, InFormat, std::forward<Args>(InArgs)...);
}

template<typename... Args>
inline void Warn(const std::string_view& InFormat, Args&&... InArgs)
{
	Logf(ESeverityFlagBits::Warn, InFormat, std::forward<Args>(InArgs)...);
}

template<typename... Args>
inline void Error(const std::string_view& InFormat, Args&&... InArgs)
{
	Logf(ESeverityFlagBits::Error, InFormat, std::forward<Args>(InArgs)...);
}

template<typename... Args>
inline void Fatal(const std::string_view& InFormat, Args&&... InArgs)
{
	Logf(ESeverityFlagBits::Fatal, InFormat, std::forward<Args>(InArgs)...);
}

/**
 * Get severity flag bit as string
 */
inline std::string_view GetSeverityAsString(const ESeverityFlagBits& InFlagBits)
{
	switch(InFlagBits)
	{
	case ESeverityFlagBits::Verbose:
		return "VERBOSE";
	default:
	case ESeverityFlagBits::Info:
		return "INFO";
	case ESeverityFlagBits::Warn:
		return "WARN";
	case ESeverityFlagBits::Error:
		return "ERROR";
	case ESeverityFlagBits::Fatal:
		return "FATAL";
	}
}
}