#pragma once

#include "VulkanCore.h"
#include "Render/Commands/RenderCommandContext.h"

class CVulkanCommandPool;
class CVulkanCommandBuffer;
class CVulkanDevice;
class CVulkanQueue;
class CVulkanCommandBufferManager;
class IRenderSystemUniformBuffer;
class CVulkanPipeline;
class CVulkanRenderCommandContext;

/**
 * Class that manage descriptor sets
 */
class CVulkanRenderCommandContextPipelineSetManager
{
public:
	CVulkanRenderCommandContextPipelineSetManager(CVulkanRenderCommandContext* InCtxt);

	/**
	 * Get active descriptor set for set X
	 * Allocate it if not found
	 */
	vk::DescriptorSet GetDescriptorSet(const uint32_t& InSet);

	/**
	 * Reset all sets
	 */
	void Reset();

	const auto& GetDescriptorSets() const { return DescriptorSets; }
private:
	CVulkanRenderCommandContext* Context;
	std::map<uint32_t, vk::DescriptorSet> DescriptorSets;
};

/**
 * Vulkan render command context
 */
class CVulkanRenderCommandContext : public IRenderCommandContext
{
	friend CVulkanRenderCommandContextPipelineSetManager;

public:
	CVulkanRenderCommandContext(CVulkanDevice* InDevice,
		CVulkanQueue* InQueue);

	virtual void Begin() override;
	virtual void End() override;
	virtual void BeginRenderPass(const std::array<float, 4>& InClearColor,
		const std::string& InName) override;
	virtual void EndRenderPass() override;
	virtual void BindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline) override;
	virtual void BindVertexBuffers(const std::vector<CRenderSystemBuffer*>& InVertexBuffers) override;
	virtual void BindIndexBuffer(CRenderSystemBuffer* InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) override;
	virtual void SetShaderUniformBuffer(const uint32_t& InSet,
		const uint32_t& InBinding, IRenderSystemUniformBuffer* InUBO) override;
	virtual void SetShaderCombinedImageSampler(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemTextureView* InView) override;
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;
	virtual void DrawIndexed(const uint32_t& InIndexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstIndex, const int32_t& InVertexOffset, const uint32_t& InFirstInstance) override;

	CVulkanCommandBufferManager* GetCommandBufferManager() const { return CommandBufferManager.get(); }
	CVulkanQueue* GetQueue() const { return Queue; }
private:
	void BindDescriptorSets();
private:
	CVulkanDevice* Device;
	CVulkanQueue* Queue;
	std::unique_ptr<CVulkanCommandBufferManager> CommandBufferManager;
	CVulkanPipeline* CurrentPipeline;
	CVulkanRenderCommandContextPipelineSetManager SetManager;
};