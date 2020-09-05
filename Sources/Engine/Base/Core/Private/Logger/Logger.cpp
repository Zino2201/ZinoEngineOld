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

namespace ZE::Logger
{

std::mutex LoggerMutex;
std::vector<std::unique_ptr<CSink>> Sinks;

/**
 * Display a message box for the specified message
 */
void MsgBox(const SMessage& InMessage)
{
#ifdef _WIN32
	UINT Type = MB_OK;

	switch(InMessage.Severity)
	{
	case ESeverityFlagBits::Verbose:
	case ESeverityFlagBits::Info:
		Type |= MB_ICONINFORMATION;
		break;
	case ESeverityFlagBits::Warn:
		Type |= MB_ICONWARNING;
		break;
	case ESeverityFlagBits::Error:
	case ESeverityFlagBits::Fatal:
		Type |= MB_ICONERROR;
		break;
	}

	MessageBoxA(nullptr, InMessage.Message.c_str(), 
		"ZINOENGINE", Type);
#endif
}

void Log(ESeverityFlagBits InSeverity, const std::string& InMessage)
{
	std::lock_guard<std::mutex> Guard(LoggerMutex);

	SMessage Message(
		std::chrono::system_clock::now(),
		std::this_thread::get_id(),
		InSeverity,
		std::move(InMessage));

	/**
	 * Call sinks
	 */
	for(const auto& Sink : Sinks)
	{
		if(Sink->GetSeverityFlags() & InSeverity)
			Sink->Log(Message);
	}

	if(InSeverity == ESeverityFlagBits::Fatal)
	{
		MsgBox(Message);
#if ZE_DEBUG
		ZE_DEBUGBREAK();
#endif
		App::Exit(-1);
	}
}

/**
 * Sink manipulation
 */
void AddSink(std::unique_ptr<CSink>&& InSink)
{
	/**
	 * Scope so that we can print a verbose message without making a infinite mutex loop
	 */
	{
		std::lock_guard<std::mutex> Guard(LoggerMutex);
		Sinks.push_back(std::move(InSink));
	}

	ZE::Logger::Verbose("Added sink {}", Sinks.back()->GetName());
}

}