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
	virtual void BindGraphicsPipeline(IGraphicsPipeline* InGraphicsPipeline) override;
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;

	CVulkanCommandBufferManager* GetCommandBufferManager() const { return CommandBufferManager.get(); }
	CVulkanQueue* GetQueue() const { return Queue; }
private:
	CVulkanDevice* Device;
	CVulkanQueue* Queue;
	std::unique_ptr<CVulkanCommandBufferManager> CommandBufferManager;
};