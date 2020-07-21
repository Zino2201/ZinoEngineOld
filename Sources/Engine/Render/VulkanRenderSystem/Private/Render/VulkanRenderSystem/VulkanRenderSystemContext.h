#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystemContext.h"

class CVulkanDevice;
class CVulkanCommandBuffer;
class CVulkanPipelineLayout;
class CVulkanSurface;

/**
 * Command buffer manager
 */
class CVulkanCommandBufferManager
{
public:
    CVulkanCommandBufferManager(CVulkanDevice& InDevice);
    ~CVulkanCommandBufferManager();

    void BeginMemoryCmdBuffer();
    void SubmitMemoryCmdBuffer();
    void SubmitGraphicsCmdBuffer(const vk::Semaphore& InSemaphore);

    CVulkanCommandBuffer* GetMemoryCmdBuffer() { return MemoryCmdBuffer.get(); }
    CVulkanCommandBuffer* GetGraphicsCmdBuffer() { return GraphicsCmdBuffer.get(); }
private:
    void SubmitCmdBuffer(CVulkanCommandBuffer* InCommandBuffer,
        const vk::Semaphore& InSemaphore);
private:
    CVulkanDevice& Device;

    vk::UniqueCommandPool CommandPool;

    /**
     * Buffer used for memory operations
     */
    std::unique_ptr<CVulkanCommandBuffer> MemoryCmdBuffer;

    /**
     * Buffer used for graphics operations
     */
    std::unique_ptr<CVulkanCommandBuffer> GraphicsCmdBuffer;
};

class CVulkanRenderSystemContext : public IRenderSystemContext
{
    friend class CVulkanPipelineSetManager;

public:
    CVulkanRenderSystemContext(CVulkanDevice& InDevice);
    ~CVulkanRenderSystemContext();

	void BeginRenderPass(const SRSRenderPass& InRenderPass,
        const SRSFramebuffer& InFrameBuffer,
		const std::array<float, 4>& InClearColor = { 0, 0, 0, 1 },
        const char* InName = "Unnamed Render Pass") override;

	void EndRenderPass() override;

    bool BeginSurface(CRSSurface* InSurface) override;
    void PresentSurface(CRSSurface* InSurface) override;

	/** States commands */
    void BindGraphicsPipeline(const SRSGraphicsPipeline& InGraphicsPipeline) override;
    void SetViewports(const std::vector<SViewport>& InViewports) override;
    void SetScissors(const std::vector<SRect2D>& InScissors) override;

    /** Resource bindings */
    void BindVertexBuffers(const std::vector<CRSBuffer*> InVertexBuffers) override;
	void BindIndexBuffer(CRSBuffer* InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) override;
    void SetShaderUniformBuffer(const uint32_t& InSet, const uint32_t& InBinding,
        CRSBuffer* InBuffer) override;
	void SetShaderTexture(const uint32_t& InSet, const uint32_t& InBinding,
		CRSTexture* InTexture) override;
	void SetShaderSampler(const uint32_t& InSet, const uint32_t& InBinding,
		CRSSampler* InSampler) override;

	/** Draw commands */
	void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;
	void DrawIndexed(const uint32_t& InIndexCount,
		const uint32_t& InInstanceCount, const uint32_t& InFirstIndex,
		const int32_t& InVertexOffset, const uint32_t& InFirstInstance) override;

    CVulkanCommandBufferManager& GetCmdBufferMgr() { return CmdBufferMgr; }
    CVulkanSurface* GetCurrentSurface() const { return CurrentSurface; }
private:
    void FlushWrites();
    void BindDescriptorSets();
private:
    CVulkanDevice& Device;
    CVulkanCommandBufferManager CmdBufferMgr;
    CVulkanPipelineLayout* CurrentLayout;
    vk::RenderPass CurrentRenderPass;
    uint32_t ColorAttachmentsCount;
    CVulkanSurface* CurrentSurface;

    struct SDescriptorSetWrite
    {
        vk::DescriptorType Type;
        union
        {
            vk::DescriptorBufferInfo BufferInfo; 
            vk::DescriptorImageInfo ImageInfo;
        };
        uint32_t Binding;
        uint32_t Count;

        SDescriptorSetWrite() : Binding(-1) {}

        SDescriptorSetWrite(const vk::DescriptorType& InType,
            const vk::DescriptorBufferInfo& InBufferInfos,
            const uint32_t& InBinding,
            const uint32_t& InCount) :
            Type(InType), BufferInfo(InBufferInfos), Binding(InBinding),
            Count(InCount) {}

		SDescriptorSetWrite(const vk::DescriptorType& InType,
			const vk::DescriptorImageInfo& InImageInfo,
			const uint32_t& InBinding,
			const uint32_t& InCount) :
			Type(InType), ImageInfo(InImageInfo), Binding(InBinding),
			Count(InCount) {}
    };

    void AddWrite(const uint32_t& InSet, const uint32_t& InBinding, 
        const SDescriptorSetWrite& InWrite, const uint64_t& InHandle);

    /** Writes & handles collected before a draw call for each set */
    robin_hood::unordered_map<uint32_t, std::array<SDescriptorSetWrite, GMaxBindingsPerSet>> WriteMap;
    robin_hood::unordered_map<uint32_t, std::array<uint64_t, GMaxBindingsPerSet>> HandleMap;
};