#pragma once

#include "EngineCore.h"

enum class ELogSeverity
{
	Debug,
	Info,
	Warn,
	Error,
	Fatal
};

/**
 * Logger singleton
 */
class CLogger
{
public:
	static CLogger& Get()
	{
		static CLogger Instance;
		return Instance;
	}

	void Log(const ELogSeverity& InSeverity, const std::string& InMessage, va_list InArgs) const;

	/**
	 * Log macro implementation
	 */
	__forceinline void LogMacroImpl(const ELogSeverity& InSeverity, const std::string InMessage, ...) const
	{
		va_list Va;
		va_start(Va, InMessage);
		Log(InSeverity, InMessage, Va);
		va_end(Va);

		if (InSeverity == ELogSeverity::Fatal)
		{
 			__debugbreak();
			system("pause");
			exit(-1);
		}
	}

	/**
	 * Convert Severity enum to string
	 */
	std::string SeverityToString(const ELogSeverity& InSeverity) const;
public:
	CLogger(const CLogger&) = delete;
	void operator=(const CLogger&) = delete;
private:
	CLogger();
	~CLogger();
};

#define LOG(Severity, Message, ...) CLogger::Get().LogMacroImpl(Severity, Message, __VA_ARGS__);