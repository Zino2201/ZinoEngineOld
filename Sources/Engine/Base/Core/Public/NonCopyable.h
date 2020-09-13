#pragma once

#include "MinimalMacros.h"

/**
 * Base class for non copyable objects
 */
class CORE_API CNonCopyable
{
public:
	CNonCopyable() = default;

	CNonCopyable(const CNonCopyable&) = delete;
	void operator=(const CNonCopyable&) = delete;
};