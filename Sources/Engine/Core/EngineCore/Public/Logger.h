#pragma once

#include "EngineCore.h"
#include <cstdarg>
#include <string>

namespace ZE::Serialization { class CFileArchive; }

namespace ZE
{

enum class ELogSeverity
{
	Debug,
	Info,
	Warn,
	Error,
	Fatal,
	FatalRetryDebug
};

#define DECLARE_LOG_CATEGORY(Name) const std::string LogCategory_##Name = #Name

DECLARE_LOG_CATEGORY(None);
DECLARE_LOG_CATEGORY(Engine);

struct SLoggerMessage
{
	ELogSeverity Severity;
	std::string Message;

	SLoggerMessage(const ELogSeverity& InSeverity,
		const std::string& InMessage) : Severity(InSeverity), Message(InMessage) {}
};

/**
 * Logger singleton
 */
class ENGINECORE_API CLogger
{
public:
	static CLogger& Get()
	{
		static CLogger Instance;
		return Instance;
	}

	void Initialize();

	void Log(const ELogSeverity& InSeverity, const std::string& InCategory, 
		const std::string& InMessage, va_list InArgs);

	/**
	 * Log macro implementation
	 */
	__forceinline void LogMacroImpl(const ELogSeverity& InSeverity, const std::string& InCategory,
		const std::string InMessage, ...)
	{
		va_list Va;
		va_start(Va, InMessage);
		Log(InSeverity, InCategory, InMessage, Va);
		va_end(Va);
	}

	/**
	 * Convert Severity enum to string
	 */
	std::string SeverityToString(const ELogSeverity& InSeverity) const;

	const auto& GetMessages() const { return Messages; }
public:
	CLogger(const CLogger&) = delete;
	void operator=(const CLogger&) = delete;
private:
	CLogger();
	~CLogger();
private:
	std::unique_ptr<Serialization::CFileArchive> FileArchive;
	std::vector<SLoggerMessage> Messages;
};

} /* namespace ZE */

#define LOG(Severity, Category, Message, ...) ZE::CLogger::Get().LogMacroImpl(Severity, LogCategory_##Category, Message, __VA_ARGS__)