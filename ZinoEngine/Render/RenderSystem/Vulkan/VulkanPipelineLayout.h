#pragma once

#include "VulkanDeviceResource.h"

struct SVulkanDescriptorPoolEntry
{
	vk::UniqueDescriptorPool DescriptorPool;
	uint32_t Allocations;
};

/**
 * Descriptor pool manager
 * Manage multiple pool at once
 */
class CVulkanDescriptorPoolManager
{
public:
	CVulkanDescriptorPoolManager(const uint32_t& InMaxSetCountPerPool,
		const std::vector<vk::DescriptorPoolSize>& InPoolSizes,
		const std::map<uint32_t, vk::DescriptorSetLayout>& InSetLayouts);

	vk::DescriptorSet AllocateSet(const uint32_t& InSet);
	void ResetPools();
private:
	void CreatePool();
	bool FindFreePool(uint32_t& InIndex);
private:
	uint32_t MaxSetCountPerPool;
	std::vector<vk::DescriptorPoolSize> PoolSizes;

	/** <Frame, Pools> */
	std::map<uint32_t, std::vector<SVulkanDescriptorPoolEntry>> DescriptorPools;

	/** One set layout per frame */
	std::map<uint32_t, std::vector<vk::DescriptorSetLayout>> SetLayouts;

	/** Current pool index for frame */
	std::map<uint32_t, uint32_t> CurrentPoolForFrame;
};

struct SVulkanPipelineLayoutInfos
{
	std::vector<vk::DescriptorSetLayoutBinding> SetLayoutBindings;
	std::map<uint32_t, vk::DescriptorSetLayout> SetLayouts;
};

class CVulkanPipelineLayout : public CVulkanDeviceResource
{
public:
	CVulkanPipelineLayout(CVulkanDevice* InDevice,
		const SVulkanPipelineLayoutInfos& InInfos);
	~CVulkanPipelineLayout();

	const vk::PipelineLayout& GetPipelineLayout() const { return *PipelineLayout; }
	const vk::DescriptorPool& GetDescriptorPool() const { return *DescriptorPool; }
	CVulkanDescriptorPoolManager* GetDescriptorPoolManager() const { return DescriptorPoolManager.get(); }

	static std::vector<CVulkanPipelineLayout*> PipelineLayouts;
private:
	vk::UniquePipelineLayout PipelineLayout;
	vk::UniqueDescriptorPool DescriptorPool;
	std::unique_ptr<CVulkanDescriptorPoolManager> DescriptorPoolManager;
};