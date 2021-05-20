#pragma once

#include "MinimalMacros.h"
#include "Severity.h"
#include <string>

namespace ze::logger
{

struct Message;

/**
 * Abstract class for a Logger sink
 */
class CORE_API Sink
{
public:
	Sink(const std::string& in_name) : name(in_name), severity_flags(SeverityFlagBits::All) {}
	virtual ~Sink() = default;

	virtual void log(const Message& message) = 0;

	ZE_FORCEINLINE const std::string& get_name() const { return name; }
	ZE_FORCEINLINE const SeverityFlags& get_severity_flags() const { return severity_flags; }
protected:
	std::string format(const Message& message);
protected:
	std::string name;
	SeverityFlags severity_flags;
};

}