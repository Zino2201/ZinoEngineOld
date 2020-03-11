#pragma once

#include "VulkanCore.h"
#include "Render/Commands/RenderCommandContext.h"
#include "VulkanPipelineLayout.h"

/**
 * A structure that englobe a WriteDescriptorSet and BufferInfo/ImageInfo
 */
struct SVulkanWriteSet
{
	union
	{
		vk::DescriptorBufferInfo BufferInfo;
		vk::DescriptorImageInfo ImageInfo;
	};

	vk::WriteDescriptorSet Write;

	SVulkanWriteSet() {}
};

class CVulkanCommandPool;
class CVulkanCommandBuffer;
class CVulkanDevice;
class CVulkanQueue;
class CVulkanCommandBufferManager;
class IRenderSystemUniformBuffer;
class CVulkanPipeline;
class CVulkanRenderCommandContext;
class CVulkanPipelineLayout;

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
	 * Reset set
	 */
	void ResetSet(const uint32_t& InSet);

	/**
	 * Reset all sets
	 */
	void Reset();

	void MarkAsResetable();
	void MarkAsBound(const uint32_t& InSet) { IsBoundMap[InSet] = true; }
	void ResetBoundState(const uint32_t& InSet) { IsBoundMap[InSet] = false; }
	void ResetBoundStates() { for(auto& [Set, State] : IsBoundMap) { ResetBoundState(Set); } }
	bool IsBound(const uint32_t& InSet) { return IsBoundMap[InSet]; }

	const auto& GetDescriptorSets() const { return DescriptorSets; }
private:
	CVulkanRenderCommandContext* Context;
	std::map<uint32_t, vk::DescriptorSet> DescriptorSets;
	std::map<uint32_t, bool> IsBoundMap;
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
	virtual void BindVertexBuffers(const std::vector<IRenderSystemVertexBuffer*>& InVertexBuffers) override;
	virtual void BindIndexBuffer(IRenderSystemIndexBuffer* InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) override;
	virtual void SetShaderUniformBuffer(const uint32_t& InSet,
		const uint32_t& InBinding, IRenderSystemUniformBuffer* InUBO) override;
	virtual void SetShaderCombinedImageSampler(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemTextureView* InView) override;
	virtual void SetShaderStorageBuffer(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemBuffer* InBuffer) override;
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;
	virtual void DrawIndexed(const uint32_t& InIndexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstIndex, const int32_t& InVertexOffset, const uint32_t& InFirstInstance) override;
	virtual void SetViewports(const std::vector<SViewport>& InViewports) override;
	virtual void SetScissors(const std::vector<SRect2D>& InScissors) override;


	CVulkanCommandBufferManager* GetCommandBufferManager() const { return CommandBufferManager.get(); }
	CVulkanQueue* GetQueue() const { return Queue; }
private:
	void BindDescriptorSets();
private:
	CVulkanDevice* Device;
	CVulkanQueue* Queue;
	std::unique_ptr<CVulkanCommandBufferManager> CommandBufferManager;
	CVulkanPipeline* CurrentPipeline;
	CVulkanPipelineLayout* CurrentPipelineLayout;
	CVulkanRenderCommandContextPipelineSetManager SetManager;
	std::unique_ptr<CVulkanDescriptorPoolManager> GlobalPoolManager;
	std::map<uint32_t, vk::UniqueDescriptorSetLayout> GlobalSetLayout;
	std::vector<SVulkanWriteSet> WriteDescriptorSets;
};