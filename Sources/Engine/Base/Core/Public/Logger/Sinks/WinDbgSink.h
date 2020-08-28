#pragma once

#include "Logger/Sink.h"

namespace ZE::Logger::Sinks
{

/**
 * Win32 debug sink
 * Print using OutputDebugMessageA
 */
class CORE_API CWinDbgSink : public CSink
{
public:
	CWinDbgSink(const std::string& InName) : CSink(InName) {}

	void Log(const SMessage& InMessage) override;
};

}