#pragma once

#include "VulkanCore.h"
#include "VulkanRenderSystemContext.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include <robin_hood.h>
#include "Render/RenderSystem/Resources/Framebuffer.h"

class CVulkanSwapChain;
class CVulkanQueue;

/**
 * Staging buffer manager
 * Keeps track of staging buffers
 */
class CVulkanStagingBufferManager
{
public:
    CVulkanStagingBufferManager(CVulkanDevice& InDevice);

	CVulkanInternalStagingBuffer* CreateStagingBuffer(uint64_t InSize,
		vk::BufferUsageFlags InUsageFlags);
    void ReleaseStagingBuffer(CVulkanInternalStagingBuffer* InBuffer);
    void ReleaseStagingBuffers();
private:
    CVulkanDevice& Device;
    robin_hood::unordered_set<CVulkanInternalStagingBuffer*> StagingBuffers;
    std::vector<CVulkanInternalStagingBuffer*> StagingBuffersToDelete;
};

/**
 * Class used to defer destruction to certain resources
 */
class CVulkanDeferredDestructionManager
{
public:
    enum class EHandleType
    {
        Buffer,
        Image,
        ImageView,
        Sampler
    };
    
    struct SEntry
    {
        EHandleType Type;
        VmaAllocation Allocation;
        uint64_t Handle;

        SEntry(EHandleType InType,
            const VmaAllocation& InAllocation,
            uint64_t InHandle) : Type(InType),
            Allocation(InAllocation), Handle(InHandle) {}

		SEntry(EHandleType InType,
			uint64_t InHandle) : Type(InType),
			Handle(InHandle) {}
    };
    CVulkanDeferredDestructionManager(CVulkanDevice& InDevice) : Device(InDevice) {}
    ~CVulkanDeferredDestructionManager() { DestroyResources(); }

    void EnqueueImage(EHandleType InHandleType, const VmaAllocation& InAllocation, 
        vk::Image& InHandle)
    {
        EnqueueResource(InHandleType, InAllocation, 
            reinterpret_cast<uint64_t>(static_cast<VkImage>(InHandle)));
	    InHandle = vk::Image();
    }

	void EnqueueImageView(EHandleType InHandleType, vk::ImageView& InHandle)
	{
		EnqueueResource(InHandleType, reinterpret_cast<uint64_t>(static_cast<VkImageView>(InHandle)));
	    InHandle = vk::ImageView();
    }

    template<typename CPPType, typename CType>
    void Enqueue(EHandleType InHandleType, CPPType& InHandle)
    {
		EnqueueResource(InHandleType, reinterpret_cast<uint64_t>(static_cast<CType>(InHandle)));
		InHandle = CPPType();
    }

	template<typename CPPType, typename CType>
	void Enqueue(EHandleType InHandleType, const VmaAllocation& InAllocation, CPPType& InHandle)
	{
		EnqueueResource(InHandleType, InAllocation, 
            reinterpret_cast<uint64_t>(static_cast<CType>(InHandle)));
		InHandle = CPPType();
	}

    void DestroyResources();
private:    
    void EnqueueResource(EHandleType InHandleType, 
        const VmaAllocation& InAllocation, uint64_t InHandle);
	void EnqueueResource(EHandleType InHandleType,
		uint64_t InHandle);
private:
    CVulkanDevice& Device;
    std::vector<SEntry> Entries;
};

/** Lifetime of a unused renderpass (in frames) */
static constexpr uint8_t GMaxLifetimeRenderPass = 10;

/** Lifetime of a unused framebuffer (in frames) */
static constexpr uint8_t GMaxLifetimeFramebuffer = 10;

/**
 * Render pass/framebuffer manager
 */
class CVulkanRenderPassFramebufferManager
{
    struct SRenderPassEntry
    {
        vk::UniqueRenderPass RenderPass;
        uint8_t LifetimeCounter;

		SRenderPassEntry() : LifetimeCounter(0) {}
		SRenderPassEntry(vk::UniqueRenderPass&& InRenderPass) :
            RenderPass(std::move(InRenderPass)), LifetimeCounter(0) {}
    };

    /**
     * A framebuffer entry
     * Each framebuffer is associated with a specific render pass
     */
    struct SFramebufferEntry
    {
        vk::UniqueFramebuffer Framebuffer;
        uint8_t LifetimeCounter;

        SFramebufferEntry() : LifetimeCounter(0) {}
        SFramebufferEntry(vk::UniqueFramebuffer&& InFramebuffer) :
            Framebuffer(std::move(InFramebuffer)), LifetimeCounter(0) {}
    };

    /**
     * Entry key of a frame buffer
     */
	struct SFramebufferEntryKey
	{
        vk::RenderPass RenderPass;
        std::array<uint64_t, GMaxRenderTargetPerFramebuffer> ColorRTs;
        std::array<uint64_t, GMaxRenderTargetPerFramebuffer> DepthRTs;

        bool operator==(const SFramebufferEntryKey& InOther) const
        {
            return RenderPass == InOther.RenderPass &&
                ColorRTs == InOther.ColorRTs &&
                DepthRTs == InOther.DepthRTs;
        }
	};

    /**
     * Entry key hash functor
     */
	struct SFramebufferEntryKeyHash
	{
        uint64_t operator()(const SFramebufferEntryKey& InKey) const
        {
            uint64_t Hash = 0;

			hash_combine<uint64_t, robin_hood::hash<uint64_t>>(Hash,
                reinterpret_cast<uint64_t>(static_cast<VkRenderPass>(InKey.RenderPass)));

			for (const auto& RT : InKey.ColorRTs)
			{
				hash_combine<uint64_t, robin_hood::hash<uint64_t>>(Hash, RT);
			}

			for (const auto& RT : InKey.DepthRTs)
			{
				hash_combine<uint64_t, robin_hood::hash<uint64_t>>(Hash, RT);
			}

            return Hash;
        }
	};
public:
    CVulkanRenderPassFramebufferManager(CVulkanDevice& InDevice);
    ~CVulkanRenderPassFramebufferManager();

    /**
     * Will destroy unused objects
     */
    void NewFrame();

    vk::RenderPass GetRenderPass(const SRSRenderPass& InRenderPass);
    vk::Framebuffer GetFramebuffer(const SRSFramebuffer& InFramebuffer,
        const vk::RenderPass& InRenderPass);
private:
    CVulkanDevice& Device;
    robin_hood::unordered_map<SRSRenderPass, SRenderPassEntry, SRSRenderPassHash> RenderPasses;
    robin_hood::unordered_map<SFramebufferEntryKey, SFramebufferEntry, SFramebufferEntryKeyHash> Framebuffers;
};

class CVulkanDevice
{
    struct SVMADestructor
    {
        CVulkanDevice& Device;

        SVMADestructor(CVulkanDevice& InDevice) : Device(InDevice) {}
        ~SVMADestructor();
    };

public:
    CVulkanDevice(const vk::PhysicalDevice& InPhysicalDevice);
    ~CVulkanDevice();

    void WaitIdle();
    void CreatePresentQueue(const vk::SurfaceKHR& InSurface);

    /** Managers */
    CVulkanStagingBufferManager& GetStagingBufferMgr() { return StagingBufferMgr; }
    CVulkanPipelineLayoutManager& GetPipelineLayoutMgr() { return PipelineLayoutMgr; }
    CVulkanDeferredDestructionManager& GetDeferredDestructionMgr() { return DeferredDestructionManager; }
    CVulkanPipelineManager& GetPipelineManager() { return PipelineManager; }
    CVulkanRenderPassFramebufferManager& GetRenderPassFramebufferMgr() { return RenderPassFramebufferMgr; }

    CVulkanRenderSystemContext* GetContext() const { return Context.get(); }
    const VmaAllocator& GetAllocator() const { return Allocator; }
    const vk::Device& GetDevice() const { return *Device; }
    const vk::PhysicalDevice& GetPhysicalDevice() const { return PhysicalDevice; }
    CVulkanQueue* GetGraphicsQueue() const { return GraphicsQueue.get(); }
    CVulkanQueue* GetPresentQueue() const { return PresentQueue; }
private:
    CVulkanDeferredDestructionManager DeferredDestructionManager;
    std::unique_ptr<CVulkanRenderSystemContext> Context;
    CVulkanStagingBufferManager StagingBufferMgr;
    VmaAllocator Allocator;
    vk::UniqueDevice Device;
    vk::PhysicalDevice PhysicalDevice;
    std::unique_ptr<CVulkanSwapChain> SwapChain;
    SVulkanQueueFamilyIndices QueueFamilyIndices;
    std::unique_ptr<CVulkanQueue> GraphicsQueue;
    CVulkanQueue* PresentQueue;
    CVulkanPipelineLayoutManager PipelineLayoutMgr;
    CVulkanPipelineManager PipelineManager;
    CVulkanRenderPassFramebufferManager RenderPassFramebufferMgr;
    std::unique_ptr<SVMADestructor> VmaDestructor;
};