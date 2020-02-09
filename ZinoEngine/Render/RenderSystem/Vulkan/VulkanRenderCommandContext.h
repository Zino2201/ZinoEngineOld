#pragma once

#include "Render/Commands/Commands.h"

class CVulkanCommandPool;
class CVulkanCommandBuffer;
class CVulkanDevice;
class CVulkanQueue;
class CVulkanCommandBufferManager;

/**
 * Vulkan render command context
 */
class CVulkanRenderCommandContext : public IRenderCommandContext
{
public:
	CVulkanRenderCommandContext(CVulkanDevice* InDevice,
		CVulkanQueue* InQueue);

	virtual void Begin() override;
	virtual void End() override;
	virtual void BeginRenderPass(const std::array<float, 4>& InClearColor) override;
	virtual void EndRenderPass() override;
	virtual void BindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline) override;
	virtual void BindShaderAttributesManager(
		const std::shared_ptr<IShaderAttributesManager>& InManager) override;
	virtual void BindVertexBuffers(const std::vector<CRenderSystemBuffer*>& InVertexBuffers) override;
	virtual void BindIndexBuffer(CRenderSystemBuffer* InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) override;
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;
	virtual void DrawIndexed(const uint32_t& InIndexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstIndex, const int32_t& InVertexOffset, const uint32_t& InFirstInstance) override;

	CVulkanCommandBufferManager* GetCommandBufferManager() const { return CommandBufferManager.get(); }
	CVulkanQueue* GetQueue() const { return Queue; }
private:
	CVulkanDevice* Device;
	CVulkanQueue* Queue;
	std::unique_ptr<CVulkanCommandBufferManager> CommandBufferManager;
};