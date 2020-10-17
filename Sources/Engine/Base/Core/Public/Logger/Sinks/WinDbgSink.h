#pragma once

#include "Logger/Sink.h"

namespace ze::logger
{

/**
 * Win32 debug sink
 * Print using OutputDebugMessageA
 */
class CORE_API WinDbgSink : public Sink
{
public:
	WinDbgSink(const std::string& name) : Sink(name) {}

	void log(const Message& message) override;
};

}