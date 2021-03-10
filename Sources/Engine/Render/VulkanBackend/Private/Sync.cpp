#include "Sync.h"
#include "Device.h"
#include "VulkanBackend.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
robin_hood::unordered_set<ResourceHandle> fences;
robin_hood::unordered_set<ResourceHandle> semaphores;
#endif

ResourceHandle VulkanBackend::fence_create(const bool in_is_signaled)
{
	ResourceHandle handle = create_resource<Fence>(*device, in_is_signaled);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	fences.insert(handle);
#endif

	return handle;
}

void VulkanBackend::fence_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Fence>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	fences.erase(in_handle);
#endif
}

void VulkanBackend::fence_wait_for(const std::vector<ResourceHandle>& in_fences,
	const bool in_wait_all, const uint64_t in_timeout)
{
	std::vector<vk::Fence> fences;
	fences.reserve(in_fences.size());
	for(const auto& handle : in_fences)
	{
		Fence* fence = Fence::get(handle);
		ZE_CHECKF(fence, "Invalid fence given to fence_wait_for");
		fences.emplace_back(fence->get_fence());
	}

	device->get_device().waitForFences(fences,
		in_wait_all, in_timeout);
}

void VulkanBackend::fence_reset(const std::vector<ResourceHandle>& in_fences)
{
	std::vector<vk::Fence> fences;
	fences.reserve(in_fences.size());
	for(const auto& handle : in_fences)
	{
		Fence* fence = Fence::get(handle);
		ZE_CHECKF(fence, "Invalid fence given to fence_reset");
		fences.emplace_back(fence->get_fence());
	}

	device->get_device().resetFences(fences);
}

Fence::Fence(Device& in_device, const bool in_is_signaled) :
	device(in_device)
{
	auto [result, handle] = device.get_device().createFenceUnique(
		vk::FenceCreateInfo(
			in_is_signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags()));
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create fence {}: {}", vk::to_string(result));

	fence = std::move(handle);
}

Fence* Fence::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto fence = fences.find(in_handle);
	ZE_CHECKF(fence != fences.end(), "Invalid fence");
#endif

	return get_resource<Fence>(in_handle);
}

ResourceHandle VulkanBackend::semaphore_create()
{
	ResourceHandle handle = create_resource<Semaphore>(*device);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	semaphores.insert(handle);
#endif
	
	return handle;
}

void VulkanBackend::semaphore_destroy(const ResourceHandle& in_handle)
{
	delete_resource<Semaphore>(in_handle);

#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	semaphores.erase(in_handle);
#endif
}

Semaphore::Semaphore(Device& in_device) :
	device(in_device)
{
	auto [result, handle] = device.get_device().createSemaphoreUnique(vk::SemaphoreCreateInfo());
	if(result != vk::Result::eSuccess)
		ze::logger::error("Failed to create semaphore {}: {}", vk::to_string(result));

	semaphore = std::move(handle);
}

Semaphore* Semaphore::get(const ResourceHandle& in_handle)
{
#if ZE_FEATURE(BACKEND_HANDLE_VALIDATION)
	auto semaphore = semaphores.find(in_handle);
	ZE_CHECKF(semaphore != semaphores.end(), "Invalid semaphore");
#endif

	return get_resource<Semaphore>(in_handle);
}

}