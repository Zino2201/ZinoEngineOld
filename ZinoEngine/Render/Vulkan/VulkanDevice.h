#pragma once

#include "VulkanCore.h"

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
private:
	/** Physical device */
	vk::PhysicalDevice PhysicalDevice;

	/** Queue family indices for current physical device */
	SVulkanQueueFamilyIndices QueueFamilyIndices;

	/** Graphics queue */
	vk::Queue GraphicsQueue;

	/** Present queue */
	vk::Queue PresentQueue;

	/** Logical device */
	vk::UniqueDevice Device;
};