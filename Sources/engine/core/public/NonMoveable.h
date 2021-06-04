#pragma once

/**
 * Base class for non copyable objects
 */
class NonMoveable
{
public:
	NonMoveable() = default;
	NonMoveable(NonMoveable&&) = delete;
	void operator=(NonMoveable&&) noexcept = delete;
};