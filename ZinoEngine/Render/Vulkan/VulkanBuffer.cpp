#include "VulkanBuffer.h"
#include "VulkanDevice.h"

CVulkanBuffer::CVulkanBuffer(CVulkanDevice* InDevice,
	const SBufferInfos& InInfos) : IBuffer(InInfos), CVulkanDeviceResource(InDevice)
{
	/** Create buffer */
	vk::BufferCreateInfo CreateInfo(
		vk::BufferCreateFlags(),
		InInfos.Size,
		VulkanUtil::BufferUsageFlagsToVkBufferUsageFlags(InInfos.Usage),
		vk::SharingMode::eExclusive);

	VmaAllocationCreateFlags Flags = InInfos.bUsePersistentMapping 
		? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;
	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.usage = VulkanUtil::BufferMemoryUsageToVmaMemoryUsage(InInfos.MemoryUsage);
	AllocInfo.flags = Flags;

	if(vmaCreateBuffer(Device->GetAllocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&CreateInfo),
		&AllocInfo,
		reinterpret_cast<VkBuffer*>(&Buffer),
		&Allocation,
		nullptr) != VK_SUCCESS)
		LOG(ELogSeverity::Fatal, "Failed to create Vulkan buffer")
}

CVulkanBuffer::~CVulkanBuffer() 
{
	vmaDestroyBuffer(Device->GetAllocator(),
		Buffer,
		Allocation);
}

void* CVulkanBuffer::Map()
{
	void* Data;
	vmaMapMemory(Device->GetAllocator(),
		Allocation,
		&Data);
	return Data;
}

void CVulkanBuffer::Unmap()
{
	vmaUnmapMemory(Device->GetAllocator(),
		Allocation);
}