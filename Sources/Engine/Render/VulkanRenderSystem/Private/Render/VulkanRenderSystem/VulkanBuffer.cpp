#include "Render/VulkanRenderSystem/VulkanCore.h"
#include "Render/VulkanRenderSystem/VulkanBuffer.h"
#include "Render/VulkanRenderSystem/VulkanCommandBuffer.h"
#include "VulkanRenderSystem.h"

CVulkanInternalStagingBuffer::CVulkanInternalStagingBuffer(
	CVulkanDevice* InDevice,
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

	if (vmaCreateBuffer(Device->GetAllocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&CreateInfo),
		&AllocInfo,
		reinterpret_cast<VkBuffer*>(&Buffer),
		&Allocation,
		&AllocationInfo) != VK_SUCCESS)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create Vulkan internal staging buffer");
}

CVulkanInternalStagingBuffer::~CVulkanInternalStagingBuffer()
{
	vmaDestroyBuffer(Device->GetAllocator(),
		Buffer,
		Allocation);
}

CVulkanBuffer::CVulkanBuffer(
	CVulkanDevice* InDevice,
	const ERSBufferUsage& InUsageFlags,
	const ERSMemoryUsage& InMemUsage,
	const uint64_t& InSize,
	const SRSResourceCreateInfo& InInfo) :
	CVulkanDeviceResource(InDevice),
	CRSBuffer(InUsageFlags, InMemUsage, InSize, InInfo)
{
	vk::BufferUsageFlags UsageFlags;
	UsageFlags |= vk::BufferUsageFlagBits::eTransferSrc |
		vk::BufferUsageFlagBits::eTransferDst;

	if(HAS_FLAG(InUsageFlags, ERSBufferUsage::VertexBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
	}
	
	if(HAS_FLAG(InUsageFlags, ERSBufferUsage::IndexBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
	}

	if(HAS_FLAG(InUsageFlags, ERSBufferUsage::UniformBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
	}

	vk::BufferCreateInfo BufferCreateInfo(
		vk::BufferCreateFlags(),
		InSize,
		UsageFlags,
		vk::SharingMode::eExclusive);

	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
	if(HAS_FLAG(InMemUsage, ERSMemoryUsage::UsePersistentMapping))
		AllocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocInfo.usage = VulkanUtil::BufferUsageFlagsToMemoryUsage(InMemUsage);
	AllocInfo.pUserData = reinterpret_cast<void*>(const_cast<char*>(InInfo.DebugName));

	vk::Result Result = static_cast<vk::Result>(vmaCreateBuffer(Device->GetAllocator(),
		reinterpret_cast<VkBufferCreateInfo*>(&BufferCreateInfo),
		&AllocInfo,
		reinterpret_cast<VkBuffer*>(&Buffer),
		&Allocation,
		&AllocationInfo));

	if (Result != vk::Result::eSuccess)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create Vulkan buffer: %s",
			vk::to_string(Result).c_str());

	/** Copy initial data to buffer */
	if (InInfo.InitialData)
	{
		void* Dst = Map(ERSBufferMapMode::WriteOnly);
		memcpy(Dst, InInfo.InitialData, InSize);
		Unmap();
	}
}

CVulkanBuffer::~CVulkanBuffer() 
{
	Device->GetDeferredDestructionMgr().Enqueue<vk::Buffer, VkBuffer>(
		CVulkanDeferredDestructionManager::EHandleType::Buffer,
		Allocation,
		Buffer);
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
			Device->GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

		Device->GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

		/**
		 * Create a transfer dst staging buffer and copy buffer content to staging
		 */
		CVulkanInternalStagingBuffer* StagingBuffer = 
			Device->GetStagingBufferMgr()->CreateStagingBuffer(Size,
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
			Size);

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
			{ vk::BufferCopy(0, 0, Size) });

		/** Wait for copy to be finished */
		Barrier = vk::BufferMemoryBarrier(
			vk::AccessFlagBits::eMemoryWrite,
			vk::AccessFlagBits::eHostRead,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			StagingBuffer->GetBuffer(),
			0,
			Size);

		CmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eHost,
			vk::DependencyFlags(),
			{},
			{ Barrier },
			{});

		Device->GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
		Device->WaitIdle();

		CurrentStagingBuffer.StagingBuffer = StagingBuffer;
		CurrentStagingBuffer.MapMode = InMapMode;

		return StagingBuffer->GetAllocationInfo().pMappedData;
	}
	else
	{
		/**
		 * Create a staging buffer that wlil be writted
		 */
		CVulkanInternalStagingBuffer* StagingBuffer =
			Device->GetStagingBufferMgr()->CreateStagingBuffer(Size,
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
		Device->GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	if(CurrentStagingBuffer.MapMode == ERSBufferMapMode::ReadOnly)
	{
		Device->GetStagingBufferMgr()->ReleaseStagingBuffer(CurrentStagingBuffer.StagingBuffer);
	}
	else
	{
		Device->GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

		/**
		 * Copy staging buffer to actual buffer
		 */
		CmdBuffer.copyBuffer(
			CurrentStagingBuffer.StagingBuffer->GetBuffer(),
			Buffer,
			{ vk::BufferCopy(0, 0, Size) });
		Device->GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
		Device->WaitIdle();
		Device->GetStagingBufferMgr()->ReleaseStagingBuffer(CurrentStagingBuffer.StagingBuffer);
	}

	CurrentStagingBuffer.StagingBuffer = nullptr;
}

CRSBuffer* CVulkanRenderSystem::CreateBuffer(
	const ERSBufferUsage& InUsageFlags,
	const ERSMemoryUsage& InMemUsage,
	const uint64_t& InSize,
	const SRSResourceCreateInfo& InInfo) const
{
	return new CVulkanBuffer(
		Device.get(),
		InUsageFlags,
		InMemUsage,
		InSize,
		InInfo);
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