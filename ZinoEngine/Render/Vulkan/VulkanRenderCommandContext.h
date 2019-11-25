#pragma once

#include "Render/Commands/Commands.h"

class CVulkanCommandPool;
class CVulkanCommandBuffer;
/**
 * Vulkan render command context
 */
class CVulkanRenderCommandContext : public IRenderCommandContext
{
public:
	CVulkanRenderCommandContext();

	virtual void Begin() override;
	virtual void End() override;
	virtual void BeginRenderPass(const std::array<float, 4>& InClearColor) override;
	virtual void EndRenderPass() override;
	virtual void BindGraphicsPipeline(IGraphicsPipeline* InGraphicsPipeline) override;
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;

	CVulkanCommandBuffer* GetCommandBuffer() const { return CommandBuffer.get(); }
private:
	std::unique_ptr<CVulkanCommandPool> CommandPool;
	std::unique_ptr<CVulkanCommandBuffer> CommandBuffer;
};