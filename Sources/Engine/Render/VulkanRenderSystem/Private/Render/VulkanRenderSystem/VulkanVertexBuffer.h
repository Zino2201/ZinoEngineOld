//#pragma once
//
//#include "VulkanCore.h"
//#include "VulkanBuffer.h"
//
//class CVulkanVertexBuffer : public CRSVertexBuffer,
//	public CVulkanBuffer
//{
//public:
//	CVulkanVertexBuffer(CVulkanDevice* InDevice, 
//		const uint64_t& InSize, ERSMemoryUsage InUsage,
//		const SRSResourceCreateInfo& InInfo);
//
//	virtual void* Map(ERSBufferMapMode InMapMode) override;
//	virtual void Unmap() override;
//};