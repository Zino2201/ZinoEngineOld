#pragma once

#include "VulkanCore.h"

class CVulkanQueue;

/**
 * Vulkan device
 */
class CVulkanDevice
{
public:
	CVulkanDevice(const vk::PhysicalDevice& InPhysDevice);
	~CVulkanDevice();

	const vk::PhysicalDevice& GetPhysicalDevice() const { return PhysicalDevice; }
	const vk::Device& GetDevice() const { return *Device; }
	const SVulkanQueueFamilyIndices& GetQueueFamilyIndices() const { return QueueFamilyIndices; }
	const VmaAllocator& GetAllocator() const { return Allocator; }
	CVulkanQueue* GetGraphicsQueue() const { return GraphicsQueue.get(); }
	CVulkanQueue* GetPresentQueue() const { return PresentQueue.get(); }
private:
	/** Physical device */
	vk::PhysicalDevice PhysicalDevice;

	/** Queue family indices for current physical device */
	SVulkanQueueFamilyIndices QueueFamilyIndices;

	/** Graphics queue */
	std::unique_ptr<CVulkanQueue> GraphicsQueue;

	/** Present queue */
	std::unique_ptr<CVulkanQueue> PresentQueue;

	/** Logical device */
	vk::UniqueDevice Device;

	/** Vma Allocator */
	VmaAllocator Allocator;
};