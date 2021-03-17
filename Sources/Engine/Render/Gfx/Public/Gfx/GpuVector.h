#pragma once

#include "Gfx/Backend.h"
#include "Gfx.h"

namespace ze::gfx
{

/**
 * std::vector alterntive storing data in GPU memory that is CPU visible
 */
template<typename T, BufferUsageFlagBits Usage>
    requires std::is_standard_layout_v<T>
class GpuVector
{
public:
	GpuVector() : size(0), capacity(0), mapped_data(nullptr) {}
	~GpuVector()
	{
		//if(buffer)
			//RenderBackend::get().buffer_unmap(*buffer);
	}

	/**
	 * Add an element to the vector
	 */
	void add(const T& in_elem)
	{
		if(++size > capacity)
			reserve(capacity + 1);

		new (&at(size - 1)) T(in_elem);
	}

	template<typename... Args>
	void add(Args&&... in_args)
	{
		if(++size > capacity)
			reserve(capacity + 1);

		new (&at(size - 1)) T(std::forward<Args>(in_args)...);
	}

	ZE_FORCEINLINE T& at(const size_t& in_idx)
	{
		ZE_CHECK(in_idx <= size - 1);
		return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(mapped_data) + (sizeof(T) * in_idx));
	}

	/*
	 * Pre-allocate data
	 * If in_capacity > get_capacity(), the buffer will be reallocated else it will do nothing
	 * \warn Old memory will only be available when the GPU will have executed the copy command!
	 */
	void reserve(const size_t in_capacity)
	{
	    if(capacity >= in_capacity)
	        return;

#if 0
		/** If we have old data to copy, copy it */
		if(buffer)
		{
			RenderBackend::get().buffer_unmap(*buffer);
			ResourceHandle old_buffer = buffer.free();
			buffer = RenderBackend::get().buffer_create(
				BufferCreateInfo(in_capacity * sizeof(T), 
					Usage | BufferUsageFlagBits::TransferSrc | BufferUsageFlagBits::TransferDst,
					MemoryUsage::CpuToGpu));

			auto& list = hlcs::allocate_cmd_list(true);
			list.add_owned_buffer(old_buffer);
			list.begin();
			list.enqueue<hlcs::CommandCopyBuffer>(
				old_buffer, 
				*buffer, 
				std::vector<BufferCopyRegion>(
					{
						BufferCopyRegion(0, 0, capacity * sizeof(T))
					}));
			list.end();

			UniqueFence fence(RenderBackend::get().fence_create(false));
			hlcs::submit(list, *fence);
			RenderBackend::get().fence_wait_for({ *fence });
		}
		else
		{
			buffer = RenderBackend::get().buffer_create(
				BufferCreateInfo(in_capacity * sizeof(T), 
					Usage | BufferUsageFlagBits::TransferSrc | BufferUsageFlagBits::TransferDst,
					MemoryUsage::CpuToGpu));
		}

		capacity = in_capacity;
		auto [map_result, map_data] = RenderBackend::get().buffer_map(*buffer);
		mapped_data = map_data;
#endif
	}

	ZE_FORCEINLINE T& operator[](const size_t& in_idx)
	{
		return at(in_idx);
	}

	ZE_FORCEINLINE const size_t& get_size() const { return size; }
	ZE_FORCEINLINE const size_t& get_capacity() const { return capacity; }
	//ZE_FORCEINLINE const ResourceHandle& get_handle() const { return *buffer; }
private:
	UniqueBuffer buffer;
	size_t size;
	size_t capacity;
	void* mapped_data;
};

template<typename T>
using GpuVertexBuffer = GpuVector<T, BufferUsageFlagBits::VertexBuffer>;

template<typename T>
using GpuIndexBuffer = GpuVector<T, BufferUsageFlagBits::IndexBuffer>;

template<typename T>
using GpuUbo = GpuVector<T, BufferUsageFlagBits::UniformBuffer>;

template<typename T>
using GpuSsbo = GpuVector<T, BufferUsageFlagBits::StorageBuffer>;

}