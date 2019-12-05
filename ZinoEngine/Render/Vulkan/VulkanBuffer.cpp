#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderSystem.h"
#include "VulkanQueue.h"
#include "VulkanTexture.h"

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
		&AllocationInfo) != VK_SUCCESS)
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

void CVulkanBuffer::Copy(IBuffer* InDst)
{
	CVulkanBuffer* Dst = static_cast<CVulkanBuffer*>(InDst);

	/** Allocate temporary command buffer from memory pool */
	vk::CommandBufferAllocateInfo AllocateInfo(
		g_VulkanRenderSystem->GetMemoryPool()->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	vk::UniqueCommandBuffer CommandBuffer = 
		std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo).value.front());

	CommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));;
	CommandBuffer->copyBuffer(Buffer, Dst->GetBuffer(), vk::BufferCopy(0, 0, Infos.Size));
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

void CVulkanBuffer::Copy(ITexture* InDst)
{
	CVulkanTexture* Dst = static_cast<CVulkanTexture*>(InDst);

	/** Transition image layout before copying */
	Dst->TransitionImageLayout(vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	/** Allocate temporary command buffer from memory pool */
	vk::CommandBufferAllocateInfo AllocateInfo(
		g_VulkanRenderSystem->GetMemoryPool()->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	vk::UniqueCommandBuffer CommandBuffer =
		std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo).value.front());

	CommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));;
	CommandBuffer->copyBufferToImage(Buffer,
		Dst->GetImage(),
		vk::ImageLayout::eTransferDstOptimal,
		{ vk::BufferImageCopy(0, 0, 0, 
			vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
				0,
				0,
				1),
			vk::Offset3D(),
			vk::Extent3D(Dst->GetInfos().Width, Dst->GetInfos().Height, Dst->GetInfos().Depth)) });
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

	/** Transition image layout after copying */
	if(HAS_FLAG(Dst->GetInfos().UsageFlags, ETextureUsage::Sampled))
		Dst->TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::eShaderReadOnlyOptimal);
}

void* CVulkanBuffer::GetMappedMemory() const
{
	return AllocationInfo.pMappedData;
}