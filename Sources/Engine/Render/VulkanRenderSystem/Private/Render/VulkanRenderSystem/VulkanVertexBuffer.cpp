//#include "VulkanVertexBuffer.h"
//#include "VulkanRenderSystem.h"
//
//CVulkanVertexBuffer::CVulkanVertexBuffer(CVulkanDevice* InDevice,
//	const uint64_t& InSize, ERSMemoryUsage InUsage,
//	const SRSResourceCreateInfo& InInfo)
//	: CRSVertexBuffer(InSize, InUsage, InInfo),
//	CVulkanBuffer(InDevice, InSize, 
//		vk::BufferUsageFlagBits::eVertexBuffer,
//		InUsage,
//		InInfo)
//{
//
//}
//
//void* CVulkanVertexBuffer::Map(ERSBufferMapMode InMapMode)
//{
//	return CVulkanBuffer::Map(InMapMode);
//}
//
//void CVulkanVertexBuffer::Unmap()
//{
//	CVulkanBuffer::Unmap();
//}
//
//CRSVertexBuffer* CVulkanRenderSystem::CreateVertexBuffer(const uint64_t& InSize,
//	ERSMemoryUsage InUsage,
//	const SRSResourceCreateInfo& InInfo) const
//{
//	return new CVulkanVertexBuffer(Device.get(),
//		InSize, InUsage, InInfo);
//}