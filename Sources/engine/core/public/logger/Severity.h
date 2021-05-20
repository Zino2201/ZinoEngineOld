#pragma once

#include "flags/Flags.h"

namespace ze::logger
{

/**
 * Severity flag bits
 */
enum class SeverityFlagBits
{
	None = 0,

	Verbose = 1 << 0,
	Info = 1 << 1,
	Warn = 1 << 2,
	Error = 1 << 3,
	Fatal = 1 << 4,

	All = Verbose | Info | Warn | Error | Fatal,
};
ENABLE_FLAG_ENUMS(SeverityFlagBits, SeverityFlags);


}