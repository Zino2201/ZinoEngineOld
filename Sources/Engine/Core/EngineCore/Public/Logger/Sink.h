#pragma once

#include "MinimalMacros.h"
#include "Severity.h"
#include <string>

namespace ZE::Logger
{

struct SMessage;

/**
 * Abstract class for a Logger sink
 */
class ENGINECORE_API CSink
{
public:
	CSink(const std::string& InName) : Name(InName), SeverityFlags(ESeverityFlagBits::All) {}
	virtual ~CSink() = default;

	virtual void Log(const SMessage& InMessage) = 0;

	inline const ESeverityFlags& GetSeverityFlags() const { return SeverityFlags; }
	inline const std::string& GetName() const { return Name; }
protected:
	std::string Format(const SMessage& InMessage);
protected:
	std::string Name;
	ESeverityFlags SeverityFlags;
};

}