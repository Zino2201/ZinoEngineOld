#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystemContext.h"

class CVulkanDevice;
class CVulkanCommandBuffer;
class CVulkanPipelineLayout;

/**
 * Command buffer manager
 */
class CVulkanCommandBufferManager
{
public:
    CVulkanCommandBufferManager(CVulkanDevice* InDevice);
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
    CVulkanDevice* Device;

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

/**
 * Manage render passes and framebuffers by hashing them
 */
class CVulkanRenderPassManager
{
public:
    CVulkanRenderPassManager(CVulkanDevice* InDevice);
    ~CVulkanRenderPassManager();

    /**
     * Get render pass handle from RenderPass structure
     * If not created, create it
     */
    vk::RenderPass GetRenderPass(const SRSRenderPass& InRenderPass);

    /**
     * Same as GetRenderPass but for framebuffer
     */
    vk::Framebuffer GetFramebuffer(const SRSFramebuffer& InFramebuffer,
        const vk::RenderPass& InRenderPass = nullptr);
private:
    CVulkanDevice* Device;
    std::unordered_map<SRSRenderPass, vk::RenderPass, SRSRenderPassHash> RenderPasses;
    std::unordered_map<SRSFramebuffer, vk::Framebuffer, SRSFramebufferHash> Framebuffers;
};

class CVulkanRenderSystemContext : public IRenderSystemContext
{
    friend class CVulkanPipelineSetManager;

public:
    CVulkanRenderSystemContext(CVulkanDevice* InDevice);
    ~CVulkanRenderSystemContext();

	virtual void BeginRenderPass(const SRSRenderPass& InRenderPass,
        const SRSFramebuffer& InFrameBuffer,
		const std::array<float, 4>& InClearColor = { 0, 0, 0, 1 }) override;

	virtual void EndRenderPass() override;

    virtual void BeginSurface(CRSSurface* InSurface) override;
    virtual void PresentSurface(CRSSurface* InSurface) override;

	/** States commands */
	virtual void BindGraphicsPipeline(CRSGraphicsPipeline* InGraphicsPipeline) override;
    virtual void SetViewports(const std::vector<SViewport>& InViewports) override;
    virtual void SetScissors(const std::vector<SRect2D>& InScissors) override;

    /** Resource bindings */
    virtual void BindVertexBuffers(const std::vector<CRSBuffer*> InVertexBuffers) override;
	virtual void BindIndexBuffer(CRSBuffer* InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) override;
    virtual void SetShaderUniformBuffer(const uint32_t& InSet, const uint32_t& InBinding,
        CRSBuffer* InBuffer) override;

	/** Draw commands */
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) override;
	virtual void DrawIndexed(const uint32_t& InIndexCount,
		const uint32_t& InInstanceCount, const uint32_t& InFirstIndex,
		const int32_t& InVertexOffset, const uint32_t& InFirstInstance) override;

    CVulkanCommandBufferManager& GetCmdBufferMgr() { return CmdBufferMgr; }
    CVulkanRenderPassManager& GetRenderPassMgr() { return RenderPassMgr; }
private:
    void FlushWrites();
private:
    CVulkanDevice* Device;
    CVulkanRenderPassManager RenderPassMgr;
    CVulkanCommandBufferManager CmdBufferMgr;
    CVulkanPipelineLayout* CurrentLayout;
    
    /**
     * Represents a descriptor set
     */
    struct SDescriptorSetEntry
    {
        /** Pool from the set was allocated */
        vk::DescriptorPool Pool;

        /** Vector of hashed resources */
        std::vector<uint64_t> Hashes;

        bool operator==(const SDescriptorSetEntry& InOther) const
        {
            uint64_t FoundValues = 0;

            for(const auto& OtherValue : InOther.Hashes)
            {
				for (const auto& MyValue : Hashes)
				{
                    if(OtherValue == MyValue)
                        FoundValues++;
				}
            }

            return FoundValues == Hashes.size();
        }
    };

	struct SDescriptorSetEntryHash 
    {
        std::size_t operator()(const SDescriptorSetEntry& InEntry) const 
        { 
			std::size_t Seed = 0;

			/** Don't hash the pool, only hash the hashes */
            for(const auto& Hash : InEntry.Hashes)
			    HashCombine(Seed, Hash);

			return Seed;
        } 
    };

	struct SDescriptorSetEntryEqual 
    { 
        bool operator()(const SDescriptorSetEntry& InLHS, const SDescriptorSetEntry& InRHS) const 
        { 
            return InLHS == InRHS; 
        } 
    };

    struct SDescriptorSetWrite
    {
        vk::DescriptorType Type;
        union
        {
            vk::DescriptorBufferInfo BufferInfo; 
        };
        uint32_t Binding;
        uint32_t Count;

        SDescriptorSetWrite(const vk::DescriptorType& InType,
            const vk::DescriptorBufferInfo& InBufferInfos,
            const uint32_t& InBinding,
            const uint32_t& InCount) :
            Type(InType), BufferInfo(InBufferInfos), Binding(InBinding),
            Count(InCount) {}
    };

    /** Map of writes for each sets */
    std::unordered_map<uint32_t, std::vector<SDescriptorSetWrite>> WriteSetMap;
    std::unordered_map<uint32_t, std::vector<uint64_t>> ResourceSetHashMap;
    std::unordered_map<SDescriptorSetEntry, vk::DescriptorSet, 
        SDescriptorSetEntryHash, SDescriptorSetEntryEqual> DescriptorSetMap;
};