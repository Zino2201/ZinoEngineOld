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
		const std::array<vk::ClearValue, 4>& InClearColor);

	/** End render pass */
	void EndRenderPass();

	/** End command buffer */
	void End();
private:
	/** Command buffer */
	vk::UniqueCommandBuffer CommandBuffer;

	/** Owner pool */
	CVulkanCommandPool* CommandPool;
};