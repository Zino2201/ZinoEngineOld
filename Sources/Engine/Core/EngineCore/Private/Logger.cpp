#include "EngineCore.h"
#include "Logger.h"
#include <iostream>
#include "Module/Module.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

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
	char PrintfBuffer[1024];

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
		vsprintf_s(PrintfBuffer, InMessage.c_str(), InArgs);
		std::cout << PrintfBuffer;
		std::cout << std::endl;
#ifdef NDEBUG
	}
#endif

	if (InSeverity >= ELogSeverity::Fatal)
	{
#ifdef _DEBUG
		if (InSeverity == ELogSeverity::FatalRetryDebug)
		{
#ifdef _WIN32
			int Ret = MessageBoxA(nullptr, PrintfBuffer, "ZINOENGINE FATAL ERROR",
				MB_RETRYCANCEL | MB_ICONERROR);
			if(Ret == IDRETRY)
			{
				return;
			}
			else
			{
				__debugbreak();
				exit(-1);
				return;
			}
		}
#endif
#endif

#ifdef _WIN32
		MessageBoxA(nullptr, PrintfBuffer, "ZINOENGINE FATAL ERROR", MB_OK | MB_ICONERROR);
#endif
		__debugbreak();
		exit(-1);
	}
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