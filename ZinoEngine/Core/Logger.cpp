#include "Logger.h"
#include <iostream>

CLogger::CLogger() {}
CLogger::~CLogger() {}

void CLogger::Log(const ELogSeverity& InSeverity, const std::string& InMessage, va_list InArgs) const
{
#ifdef NDEBUG
	if (InSeverity != ELogSeverity::Debug)
	{
#endif
		std::cout << "[" << SeverityToString(InSeverity) << "] ";
		std::vprintf(InMessage.c_str(), InArgs);
		std::cout << std::endl;
#ifdef NDEBUG
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