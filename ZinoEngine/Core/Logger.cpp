#include "Logger.h"
#include <iostream>

CLogger::CLogger() {}
CLogger::~CLogger() {}

void CLogger::Log(const ELogSeverity& InSeverity, const std::string& InMessage, va_list InArgs) const
{
#ifndef DEBUG_LOG
	if (InSeverity != ELogSeverity::Debug)
	{
#endif
		std::string ThreadName;

		if(std::this_thread::get_id() == GameThreadID)
		{
			ThreadName = "GameThread";
		}
		else if(std::this_thread::get_id() == RenderThreadID)
		{
			ThreadName = "RenderThread";
		}
		else if(std::this_thread::get_id() == StatThreadID)
		{
			ThreadName = "StatThread";
		}

		std::cout << "[" << SeverityToString(InSeverity) << "/" << ThreadName << "] ";
		std::vprintf(InMessage.c_str(), InArgs);
		std::cout << std::endl;
#ifndef DEBUG_LOG
	}
#endif
}

std::string CLogger::SeverityToString(const ELogSeverity& InSeverity) const
{
	switch (InSeverity)
	{
	case ELogSeverity::Debug:
		return "DEBUG";
	case ELogSeverity::Info:
		return "INFO";
	case ELogSeverity::Warn:
		return "WARN";
	case ELogSeverity::Error:
		return "ERROR";
	case ELogSeverity::Fatal:
		return "FATAL";
	}
	
	return "INVALID";
}