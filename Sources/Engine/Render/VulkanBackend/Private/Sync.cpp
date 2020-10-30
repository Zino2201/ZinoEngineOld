#include "Sync.h"
#include "Device.h"
#include "VulkanBackend.h"
#include <robin_hood.h>

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, Fence> fences;
robin_hood::unordered_map<ResourceHandle, Semaphore> semaphores;

ResourceHandle VulkanBackend::fence_create(const bool in_is_signaled)
{
	ResourceHandle handle;

	Fence fence(*device,
		in_is_signaled);
	if(fence.is_valid())
	{
		handle = create_resource_handle(ResourceType::Fence, 
			static_cast<VkFence>(fence.get_fence()), in_is_signaled);
		fences.insert({ handle, std::move(fence)});
	}

	return handle;
}

void VulkanBackend::fence_destroy(const ResourceHandle& in_handle)
{
	fences.erase(in_handle);
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
	auto fence = fences.find(in_handle);

	if(fence != fences.end())
		return &fence->second;
	
	return nullptr;
}

ResourceHandle VulkanBackend::semaphore_create()
{
	ResourceHandle handle;

	Semaphore semaphore(*device);
	if(semaphore.is_valid())
	{
		handle = create_resource_handle(ResourceType::Fence, 
			static_cast<VkSemaphore>(semaphore.get_semaphore()), 0);
		semaphores.insert({ handle, std::move(semaphore)});
	}
}

void VulkanBackend::semaphore_destroy(const ResourceHandle& in_handle)
{
	semaphores.erase(in_handle);
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
	auto semaphore = semaphores.find(in_handle);

	if(semaphore != semaphores.end())
		return &semaphore->second;
	
	return nullptr;
}

}