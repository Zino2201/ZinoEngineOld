#pragma once

#include "EngineCore.h"

namespace ze::gfx
{

struct ResourceHandle
{
	static constexpr uint64_t null = -1;

	uint64_t handle;

	ResourceHandle() : handle(null) {}
	explicit ResourceHandle(const uint64_t& in_handle) : handle(in_handle) {}

	ZE_FORCEINLINE operator bool() const
	{
		return handle != null;
	}

	ZE_FORCEINLINE bool operator==(const ResourceHandle& in_handle) const
	{
		return handle == in_handle.handle;
	}
};

}