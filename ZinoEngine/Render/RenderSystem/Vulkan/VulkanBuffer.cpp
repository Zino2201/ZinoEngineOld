#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderSystem.h"
#include "VulkanQueue.h"
#include "VulkanTexture.h"

CVulkanBuffer::CVulkanBuffer(CVulkanDevice* InDevice,
	const SRenderSystemBufferInfos& InInfos) : CRenderSystemBuffer(InInfos), CVulkanDeviceResource(InDevice)
{
	/** Create buffer */
	vk::BufferCreateInfo CreateInfo(
		vk::BufferCreateFlags(),
		InInfos.Size,
		VulkanUtil::BufferUsageFlagsToVkBufferUsageFlags(InInfos.Usage),
		vk::SharingMode::eExclusive);

	VmaAllocationCreateFlags Flags = InInfos.bUsePersistentMapping 
		? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;
	Flags |= VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.usage = VulkanUtil::BufferMemoryUsageToVmaMemoryUsage(InInfos.MemoryUsage);
	AllocInfo.flags = Flags;
	AllocInfo.pUserData = const_cast<char*>(InInfos.DebugName.c_str());

	if(vmaCreateBuffer(Device->GetAllocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&CreateInfo),
		&AllocInfo,
		reinterpret_cast<VkBuffer*>(&Buffer),
		&Allocation,
		&AllocationInfo) != VK_SUCCESS)
		LOG(ELogSeverity::Fatal, "Failed to create Vulkan buffer")
}

CVulkanBuffer::~CVulkanBuffer() { }

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

void CVulkanBuffer::Copy(CRenderSystemBuffer* InSrc)
{
	CVulkanBuffer* Src = static_cast<CVulkanBuffer*>(InSrc);

	/** Allocate temporary command buffer from memory pool */
	vk::CommandBufferAllocateInfo AllocateInfo(
		g_VulkanRenderSystem->GetMemoryPool()->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	vk::UniqueCommandBuffer CommandBuffer = 
		std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo).value.front());

	CommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));;
	CommandBuffer->copyBuffer(Src->GetBuffer(), Buffer, vk::BufferCopy(0, 0, Src->GetInfos().Size));
	CommandBuffer->end();

	/** Submit to graphics queue */

	vk::SubmitInfo SubmitInfo(
		0,
		nullptr,
		nullptr,
		1,
		&*CommandBuffer,
		0,
		nullptr);
	Device->GetGraphicsQueue()->GetQueue().submit(SubmitInfo, vk::Fence());
	Device->GetGraphicsQueue()->GetQueue().waitIdle();
}

void* CVulkanBuffer::GetMappedMemory() const
{
	return AllocationInfo.pMappedData;
}

void CVulkanBuffer::Destroy()
{
	LOG(ELogSeverity::Debug, "Destroyed buffer %s",
		Infos.DebugName.c_str())

	vmaDestroyBuffer(Device->GetAllocator(),
		Buffer,
		Allocation);
}