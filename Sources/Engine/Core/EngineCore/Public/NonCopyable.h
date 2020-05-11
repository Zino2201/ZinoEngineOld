#pragma once

/**
 * Base class for non copyable objects
 */
class CNonCopyable
{
public:
	CNonCopyable() = default;

	CNonCopyable(const CNonCopyable&) = delete;
	void operator=(const CNonCopyable&) = delete;
};