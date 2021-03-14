#pragma once

#include "Gfx.h"

namespace ze::gfx
{

/**
 * Helper type for managing simple uniform buffers
 */
template<typename T>
	requires std::is_standard_layout_v<T>
class UniformBuffer
{
public:
	UniformBuffer() : mapped_data(nullptr)
	{
		handle = Device::get().create_buffer(BufferInfo::make_ubo(sizeof(T))).second;
		auto map = Device::get().map_buffer(handle);
		mapped_data = map.second;
	}

	~UniformBuffer()
	{
		Device::get().unmap_buffer(handle);
		Device::get().destroy_buffer(handle);
	}

	void update(const T& in_data)
	{
		memcpy(mapped_data, &in_data, sizeof(T));
	}

	DeviceResourceHandle get() const
	{
		return handle;
	}

	operator DeviceResourceHandle()
	{
		return handle;
	}
private:
	DeviceResourceHandle handle;
	void* mapped_data;
};

}