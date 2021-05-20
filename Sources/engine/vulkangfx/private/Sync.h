#pragma once

#include "Vulkan.h"
#include "gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;

struct Fence
{
public:
	Fence(Device& in_device, 
		const bool in_is_signaled);

	static Fence* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!fence; }
	ZE_FORCEINLINE vk::Fence& get_fence() { return *fence; }
private:
	Device& device;
	vk::UniqueFence fence;
};

struct Semaphore
{
public:
	Semaphore(Device& in_device);

	static Semaphore* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const { return !!semaphore; }
	ZE_FORCEINLINE vk::Semaphore& get_semaphore() { return *semaphore; }
private:
	Device& device;
	vk::UniqueSemaphore semaphore;
};

}