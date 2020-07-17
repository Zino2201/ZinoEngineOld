#include "EngineCore.h"
#include "Logger.h"
#include <iostream>
#include <mutex>
#include <sstream>
#include "Module/Module.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include "FileSystem/ZFS.h"
#include "Serialization/FileArchive.h"
#include <filesystem>

DECLARE_LOG_CATEGORY(Logger);

namespace ZE
{

CLogger::CLogger() {}
CLogger::~CLogger() 
{
	
}

void CLogger::Initialize()
{
	std::time_t Time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm LocalTime;
	localtime_s(&LocalTime, &Time);

	// TODO: Call ZFS function
	std::filesystem::create_directories("Logs/");

	std::stringstream ss;
	ss << std::put_time(&LocalTime, "/Logs/ZinoEngine_%H_%M_%S.log");

	TOwnerPtr<FileSystem::IFile> File = FileSystem::Write(ss.str().c_str());
	if(File)
	{
		FileArchive = std::make_unique<Serialization::CFileArchive>(File);
	}
	else
	{
		LOG(ELogSeverity::Error, Logger, "No log file will be created !");
	}
}

void CLogger::Log(const ELogSeverity& InSeverity, 
	const std::string& InCategory, const std::string& InMessage, va_list InArgs)
{
	char PrintfBuffer[2048];

#ifdef NDEBUG
	if(InSeverity == ELogSeverity::Debug)
		return;
#endif

	static std::mutex Mutex;
	std::lock_guard<std::mutex> Guard(Mutex);

	std::time_t Time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm LocalTime;
	localtime_s(&LocalTime, &Time);

	std::stringstream FinalString;
	FinalString << "(" << std::put_time(&LocalTime, "%H:%M:%S") << ") [" 
		<< SeverityToString(InSeverity) << "/"
		<< InCategory << "] ";
	vsprintf_s(PrintfBuffer, InMessage.c_str(), InArgs);
	FinalString << PrintfBuffer;
	FinalString << "\n";

	std::string Message = FinalString.str();
	std::cout << Message;
	
#ifdef _WIN32
	OutputDebugStringA(Message.c_str());
#endif

	if(FileArchive)
	{
		Serialization::CFileArchive& FA = *FileArchive.get();
		FA << Message;
		FA.Flush();
	}
	Messages.emplace_back(InSeverity, std::move(PrintfBuffer));

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