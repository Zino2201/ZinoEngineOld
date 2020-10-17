#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/Resources/Pipeline.h"
#include "Render/RenderSystem/Resources/GraphicsPipeline.h"

/** Fwd declares */
class CVulkanPipelineLayout;
class CVulkanDevice;
class CVulkanGraphicsPipeline;

/**
 * Class that manage pipelines
 */
class CVulkanPipelineManager
{
    struct SGraphicsEntry
    {
        SRSGraphicsPipeline Pipeline;
        vk::RenderPass RenderPass;

        bool operator==(const SGraphicsEntry& InEntry) const
        {
            return Pipeline == InEntry.Pipeline &&
                RenderPass == InEntry.RenderPass;
        }
    };

	struct SGraphicsEntryHash
	{
		SRSGraphicsPipeline Pipeline;
		vk::RenderPass RenderPass;

        uint64_t operator()(const SGraphicsEntry& InEntry) const
        {
            uint64_t Hash = 0;

            hash_combine<SRSGraphicsPipeline, SRSGraphicsPipelineHash>(Hash, InEntry.Pipeline);
            hash_combine(Hash, 
                reinterpret_cast<uint64_t>(static_cast<VkRenderPass>(InEntry.RenderPass)));

            return Hash;
        }
	};

public:
    CVulkanPipelineManager(CVulkanDevice& InDevice);

    CVulkanGraphicsPipeline* GetOrCreateGraphicsPipeline(const SRSGraphicsPipeline& InPipeline,
        const vk::RenderPass& InRenderPass);
private:
    CVulkanDevice& Device;
    robin_hood::unordered_map<SGraphicsEntry, std::unique_ptr<CVulkanGraphicsPipeline>, 
        SGraphicsEntryHash> GraphicsPipelines;
};

/**
 * Basic vulkan pipeline
 */
class CVulkanPipeline : public CVulkanDeviceResource
{
public:
    CVulkanPipeline(CVulkanDevice& InDevice);

    virtual const vk::Pipeline& GetPipeline() const { return *Pipeline; }
    virtual CVulkanPipelineLayout* GetPipelineLayout() const { return Layout.get(); }
protected:
    vk::UniquePipeline Pipeline;
    boost::intrusive_ptr<CVulkanPipelineLayout> Layout;
};

/**
 * Graphics pipeline
 */
class CVulkanGraphicsPipeline : public CVulkanPipeline
{
public:
    CVulkanGraphicsPipeline(CVulkanDevice& InDevice,
        const SRSGraphicsPipeline& InGraphicsPipeline,
        const vk::RenderPass& InRenderPass);
};