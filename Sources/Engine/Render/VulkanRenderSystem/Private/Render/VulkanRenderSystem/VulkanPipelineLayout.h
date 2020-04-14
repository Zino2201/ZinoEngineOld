#pragma once

#include "VulkanCore.h"

/** Fwd declares */
class CVulkanPipelineLayout;
using CVulkanPipelineLayoutPtr = boost::intrusive_ptr<CVulkanPipelineLayout>;

/**
 * Bindings for a single set
 */
struct SVulkanDescriptorSetBindings
{
	std::vector<vk::DescriptorSetLayoutBinding> Bindings;

	bool operator==(const SVulkanDescriptorSetBindings& InOther) const
	{
		if (Bindings.size() != InOther.Bindings.size())
			return false;

		int i = 0;
		for (const auto& Binding : Bindings)
		{
			if (Binding != InOther.Bindings[i])
				return false;

			i++;
		}

		return true;
	}

	bool operator!=(const SVulkanDescriptorSetBindings& InOther) const
	{
		if (Bindings.size() != InOther.Bindings.size())
			return true;

		int i = 0;
		for (const auto& Binding : Bindings)
		{
			if (Binding != InOther.Bindings[i])
				return true;

			i++;
		}

		return false;
	}
};

/**
 * Describe a basic pipline layout
 */
struct SVulkanPipelineLayoutDesc
{
	std::vector<SVulkanDescriptorSetBindings> SetLayoutBindings;
	// TODO: Support push constants

	bool operator==(const SVulkanPipelineLayoutDesc& InOther) const
	{
		if(SetLayoutBindings.size() != InOther.SetLayoutBindings.size())
			return false;

		int i = 0;
		for(const auto& SetLayoutBinding : SetLayoutBindings)
		{
			if(SetLayoutBinding != InOther.SetLayoutBindings[i])
				return false;

			i++;
		}

		return true;
	}

	std::vector<vk::DescriptorSetLayoutBinding> GetAllBindings() const
	{
		std::vector<vk::DescriptorSetLayoutBinding> Bindings;

		for(const auto& Set : SetLayoutBindings)
		{
			for(const auto& Binding : Set.Bindings)
				Bindings.push_back(Binding);
		}
	}
};

namespace std
{
	template<> struct hash<SVulkanDescriptorSetBindings>
	{
		std::size_t operator()(const SVulkanDescriptorSetBindings& InBindings) const noexcept
		{
			std::size_t Seed = 0;

			HashCombine(Seed, InBindings.Bindings.size());

			for (const auto& Binding : InBindings.Bindings)
			{
				HashCombine(Seed, &Binding);
			}

			return Seed;
		}
	};

	template<> struct hash<SVulkanPipelineLayoutDesc>
	{
		std::size_t operator()(const SVulkanPipelineLayoutDesc& InEntry) const noexcept
		{
			std::size_t Seed = 0;

			for(const auto& Entry : InEntry.SetLayoutBindings)
			{
				HashCombine(Seed, Entry);
			}
	
			return Seed;
		}
	};
}

/**
 * Pipeline layout manager
 */
class CVulkanPipelineLayoutManager
{
public:
	CVulkanPipelineLayoutManager(CVulkanDevice* InDevice);
	~CVulkanPipelineLayoutManager();

	/**
	 * Get a pipeline layout from a layout entry, create it if not found
	 */
	CVulkanPipelineLayout* GetPipelineLayout(const SVulkanPipelineLayoutDesc& InEntry);
private:
	CVulkanDevice* Device;
    std::unordered_map<SVulkanPipelineLayoutDesc, CVulkanPipelineLayoutPtr> Layouts;
};

/**
 * Class that manages descriptor pool and descriptor sets
 * 
 * TODO: For now, it doesn't keep track of descriptor set that are not used anymore,
 * but in the near future, there are going to be recycled
 */
class CVulkanDescriptorCacheManager
{
	struct SDescriptorPoolEntry
	{
		vk::DescriptorPool Pool;
		uint32_t Allocations;

		SDescriptorPoolEntry() : Allocations(0) {}
		SDescriptorPoolEntry(const vk::DescriptorPool& InPoolHandle) : Pool(InPoolHandle),
			Allocations(0) {}
	};

public:
	CVulkanDescriptorCacheManager(CVulkanDevice* InDevice,
		CVulkanPipelineLayout* InLayout,
		const uint32_t& InMaxAllocationsPerPool);
	~CVulkanDescriptorCacheManager();

	/**
	 * Allocate a descriptor set for given set
	 */
	vk::DescriptorSet AllocateDescriptorSet(const uint32_t& InSet,
		vk::DescriptorPool& OutPool);
private:
	const SDescriptorPoolEntry& GetPool();
private:
	CVulkanDevice* Device;
	CVulkanPipelineLayout* Layout;
	std::vector<SDescriptorPoolEntry> Pools;
	uint32_t MaxAllocationsPerPool;
};

/**
 * A pipeline layout
 */
class CVulkanPipelineLayout : public CVulkanDeviceResource,
	public boost::intrusive_ref_counter<CVulkanPipelineLayout, boost::thread_unsafe_counter>
{
public:
	CVulkanPipelineLayout(CVulkanDevice* Device,
		const SVulkanPipelineLayoutDesc& InDesc);
	~CVulkanPipelineLayout();

	const vk::DescriptorSetLayout& GetLayoutForSet(const uint32_t& InSet) const 
	{
		const auto& Result = Layouts.find(InSet);
		return *Result->second;
	}

	const vk::PipelineLayout& GetPipelineLayout() const { return *PipelineLayout; }
	const std::vector<vk::DescriptorPoolSize>& GetPoolSizes() const { return PoolSizes; }
	CVulkanDescriptorCacheManager* GetCacheMgr() { return CacheMgr.get(); }
private:
	std::unordered_map<uint32_t, vk::UniqueDescriptorSetLayout> Layouts;
	vk::UniquePipelineLayout PipelineLayout;
	std::vector<vk::DescriptorPoolSize> PoolSizes;
	std::unique_ptr<CVulkanDescriptorCacheManager> CacheMgr;
};