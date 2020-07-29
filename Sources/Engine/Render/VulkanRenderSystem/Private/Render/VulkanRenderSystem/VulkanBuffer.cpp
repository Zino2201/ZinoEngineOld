#include "Render/VulkanRenderSystem/VulkanCore.h"
#include "Render/VulkanRenderSystem/VulkanBuffer.h"
#include "Render/VulkanRenderSystem/VulkanCommandBuffer.h"
#include "VulkanRenderSystem.h"

CVulkanInternalStagingBuffer::CVulkanInternalStagingBuffer(
	CVulkanDevice& InDevice,
	uint64_t InSize, 
	vk::BufferUsageFlags InUsageFlags) : CVulkanDeviceResource(InDevice), Size(InSize)

{
	vk::BufferCreateInfo CreateInfo(
		vk::BufferCreateFlags(),
		InSize,
		InUsageFlags,
		vk::SharingMode::eExclusive);

	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	AllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	if (vmaCreateBuffer(Device.GetAllocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&CreateInfo),
		&AllocInfo,
		reinterpret_cast<VkBuffer*>(&Buffer),
		&Allocation,
		&AllocationInfo) != VK_SUCCESS)
		ZE::Logger::Fatal("Failed to create Vulkan internal staging buffer");
}

CVulkanInternalStagingBuffer::~CVulkanInternalStagingBuffer()
{
	vmaDestroyBuffer(Device.GetAllocator(),
		Buffer,
		Allocation);
}

CVulkanBuffer::CVulkanBuffer(
	CVulkanDevice& InDevice,
	const SRSBufferCreateInfo& InCreateInfo) :
	CVulkanDeviceResource(InDevice),
	CRSBuffer(InCreateInfo)
{
	vk::BufferUsageFlags UsageFlags;
	UsageFlags |= vk::BufferUsageFlagBits::eTransferSrc |
		vk::BufferUsageFlagBits::eTransferDst;

	if(InCreateInfo.UsageFlags & ERSBufferUsageFlagBits::VertexBuffer)
	{
		UsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
	}
	
	if(InCreateInfo.UsageFlags & ERSBufferUsageFlagBits::IndexBuffer)
	{
		UsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
	}

	if(InCreateInfo.UsageFlags & ERSBufferUsageFlagBits::UniformBuffer)
	{
		UsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
	}

	vk::BufferCreateInfo BufferCreateInfo(
		vk::BufferCreateFlags(),
		InCreateInfo.Size,
		UsageFlags,
		vk::SharingMode::eExclusive);

	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
	AllocInfo.usage = VulkanUtil::BufferUsageFlagsToMemoryUsage(InCreateInfo.MemoryUsage);
	if(InCreateInfo.Hints & ERSMemoryHintFlagBits::Mapped)
		AllocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

	vk::Result Result = static_cast<vk::Result>(vmaCreateBuffer(Device.GetAllocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&BufferCreateInfo),
		&AllocInfo,
		reinterpret_cast<VkBuffer*>(&Buffer),
		&Allocation,
		&AllocationInfo));

	if (Result != vk::Result::eSuccess)
		ZE::Logger::Fatal("Failed to create Vulkan buffer: {}",
			vk::to_string(Result).c_str());
}

CVulkanBuffer::~CVulkanBuffer() 
{
	Device.GetDeferredDestructionMgr().Enqueue<vk::Buffer, VkBuffer>(
		CVulkanDeferredDestructionManager::EHandleType::Buffer,
		Allocation,
		Buffer);
}

void CVulkanBuffer::SetName(const char* InName)
{
	CRSResource::SetName(InName);

	vmaSetAllocationUserData(Device.GetAllocator(),
		Allocation, reinterpret_cast<void*>(const_cast<char*>(InName)));

#ifndef NDEBUG
	PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)
		GVulkanRenderSystem->GetInstance().getProcAddr("vkSetDebugUtilsObjectNameEXT");

	VkDebugUtilsObjectNameInfoEXT Info;
	Info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	Info.pNext = nullptr;
	Info.objectType = VK_OBJECT_TYPE_BUFFER;
	Info.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkBuffer>(Buffer));
	Info.pObjectName = InName;

	SetDebugUtilsObjectName(static_cast<VkDevice>(Device.GetDevice()), &Info);
#endif
}

void* CVulkanBuffer::Map(ERSBufferMapMode InMapMode)
{
	must(!CurrentStagingBuffer.StagingBuffer);
	/**
	 * If we are reading only, wait all operations
	 * Else, create a staging buffer to copy at the beginning of the new frame
	 */
	if(InMapMode == ERSBufferMapMode::ReadOnly)
	{
		// TODO: Check if in render pass

		const vk::CommandBuffer& CmdBuffer = 
			Device.GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

		Device.GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

		/**
		 * Create a transfer dst staging buffer and copy buffer content to staging
		 */
		CVulkanInternalStagingBuffer* StagingBuffer = 
			Device.GetStagingBufferMgr().CreateStagingBuffer(CreateInfo.Size,
				vk::BufferUsageFlagBits::eTransferDst);

		/**
		 * Wait for all transfer operations to be completed on the buffer
		 */
		vk::BufferMemoryBarrier Barrier = vk::BufferMemoryBarrier(
			vk::AccessFlagBits::eMemoryWrite, 
			vk::AccessFlagBits::eMemoryRead,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			Buffer,
			0,
			CreateInfo.Size);

		CmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eTransfer,
			vk::DependencyFlags(),
			{},
			{ Barrier },
			{});

		/** Copy to staging buffer */
		CmdBuffer.copyBuffer(Buffer,
			StagingBuffer->GetBuffer(),
			{ vk::BufferCopy(0, 0, CreateInfo.Size) });

		/** Wait for copy to be finished */
		Barrier = vk::BufferMemoryBarrier(
			vk::AccessFlagBits::eMemoryWrite,
			vk::AccessFlagBits::eHostRead,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			StagingBuffer->GetBuffer(),
			0,
			CreateInfo.Size);

		CmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eHost,
			vk::DependencyFlags(),
			{},
			{ Barrier },
			{});

		Device.GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
		Device.WaitIdle();

		CurrentStagingBuffer.StagingBuffer = StagingBuffer;
		CurrentStagingBuffer.MapMode = InMapMode;

		return StagingBuffer->GetAllocationInfo().pMappedData;
	}
	else
	{
		/**
		 * Create a staging buffer that will be writted
		 */
		CVulkanInternalStagingBuffer* StagingBuffer =
			Device.GetStagingBufferMgr().CreateStagingBuffer(CreateInfo.Size,
				vk::BufferUsageFlagBits::eTransferSrc);

		CurrentStagingBuffer.StagingBuffer = StagingBuffer;
		CurrentStagingBuffer.MapMode = InMapMode;

		return StagingBuffer->GetAllocationInfo().pMappedData;
	}
}

void CVulkanBuffer::Unmap()
{
	must(CurrentStagingBuffer.StagingBuffer);

	const vk::CommandBuffer& CmdBuffer =
		Device.GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	if(CurrentStagingBuffer.MapMode == ERSBufferMapMode::ReadOnly)
	{
		Device.GetStagingBufferMgr().ReleaseStagingBuffer(CurrentStagingBuffer.StagingBuffer);
	}
	else
	{
		Device.GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

		/**
		 * Copy staging buffer to actual buffer
		 */
		CmdBuffer.copyBuffer(
			CurrentStagingBuffer.StagingBuffer->GetBuffer(),
			Buffer,
			{ vk::BufferCopy(0, 0, CreateInfo.Size) });
		Device.GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
		Device.WaitIdle();
		Device.GetStagingBufferMgr().ReleaseStagingBuffer(CurrentStagingBuffer.StagingBuffer);
	}

	CurrentStagingBuffer.StagingBuffer = nullptr;
}

TOwnerPtr<CRSBuffer> CVulkanRenderSystem::CreateBuffer(
	const SRSBufferCreateInfo& InCreateInfo) const
{
	return new CVulkanBuffer(
		*Device.get(), InCreateInfo);
}

VmaMemoryUsage VulkanUtil::BufferUsageFlagsToMemoryUsage(ERSMemoryUsage BufferUsage)
{
	VmaMemoryUsage MemoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

	switch(BufferUsage)
	{
	case ERSMemoryUsage::DeviceLocal:
		MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		break;
	case ERSMemoryUsage::HostVisible:
		MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		break;
	case ERSMemoryUsage::HostOnly:
		MemoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
		break;
	}

	return MemoryUsage;
}