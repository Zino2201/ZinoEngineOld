#pragma once

/**
 * Base class for non copyable objects
 */
class NonCopyable
{
public:
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	void operator=(const NonCopyable&) = delete;
};