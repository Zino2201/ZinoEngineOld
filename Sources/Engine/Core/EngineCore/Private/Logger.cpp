#include "EngineCore.h"
#include "Logger.h"
#include <iostream>
#include "Module/Module.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, "EngineCore")

CLogger::CLogger() {}
CLogger::~CLogger() {}

ENGINECORE_API std::thread::id GameThreadID;
ENGINECORE_API std::thread::id RenderThreadID;
ENGINECORE_API std::thread::id StatThreadID;

void CLogger::Log(const ELogSeverity& InSeverity, 
	const std::string& InCategory, const std::string& InMessage, va_list InArgs) const
{
#ifdef NDEBUG
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
		else
		{
			ThreadName = "UnknownThread";
		}

		std::cout << "[" << SeverityToString(InSeverity) << "/" << ThreadName << "] ("
			<< InCategory << ") ";
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

} /* namespace ZE */