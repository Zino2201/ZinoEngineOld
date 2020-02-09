#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"

class CVulkanCommandPool;

/**
 * Command buffer
 */
class CVulkanCommandBuffer : public CVulkanDeviceResource
{
public:
	CVulkanCommandBuffer(CVulkanDevice* InDevice,
		CVulkanCommandPool* InCommandPool);
	~CVulkanCommandBuffer();

	/** Begin command buffer */
	void Begin();

	/** Begin renderpass */
	void BeginRenderPass(const vk::RenderPass& InRenderPass,
		const vk::Framebuffer& InFramebuffer,
		const std::array<float, 4>& InClearColor);

	/** End render pass */
	void EndRenderPass();

	/** End command buffer */
	void End();

	/**
	 * Add wait semaphore
	 */
	void AddWaitSemaphore(const vk::PipelineStageFlagBits& InStageFlag,
		const vk::Semaphore& InSemaphore);

	void SubmitSemaphores();

	const vk::CommandBuffer& GetCommandBuffer() const { return *CommandBuffer; }
	const std::vector<vk::Semaphore>& GetWaitSemaphores() const { return WaitSemaphores; }
	const vk::PipelineStageFlags& GetWaitFlags() const { return WaitFlags; }
private:
	/** Command buffer */
	vk::UniqueCommandBuffer CommandBuffer;

	/** Owner pool */
	CVulkanCommandPool* CommandPool;

	/** Wait semaphores */
	std::vector<vk::Semaphore> WaitSemaphores;

	/** Wait flags */
	vk::PipelineStageFlags WaitFlags;
};