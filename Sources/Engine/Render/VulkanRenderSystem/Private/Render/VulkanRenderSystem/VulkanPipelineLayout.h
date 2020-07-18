#pragma once

#include "VulkanCore.h"
#include <queue>
#include <robin_hood.h>

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

	auto& GetLayoutMap() { return Layouts; }
private:
	CVulkanDevice* Device;
    std::unordered_map<SVulkanPipelineLayoutDesc, CVulkanPipelineLayoutPtr> Layouts;
};

/** Lifetime of a unused descriptor set (in frames) */
static constexpr uint8_t GMaxLifetimeDescriptorSet = 10;

/** Max descriptor set per pools */
static constexpr uint8_t GMaxDescriptorSetPerPool = 32;

/**
 * Vulkan descriptor set & pool manager
 * Works by storing descriptor set and reuse them when possible to not do useless updates,
 * it also sets that are not used anymore
 */
class CVulkanDescriptorSetManager
{
	using HandlesArrayType = std::array<uint64_t, GMaxBindingsPerSet>;

	struct SDescriptorPoolEntry
	{
		uint8_t Allocations;
		vk::DescriptorPool Pool;

		SDescriptorPoolEntry() : Allocations(0) {}
		SDescriptorPoolEntry(const vk::DescriptorPool& InPool) : Pool(InPool) {}
	};

	struct SDescriptorSetEntry
	{
		uint8_t LifetimeCounter;
		vk::DescriptorSet SetHandle;

		SDescriptorSetEntry() : LifetimeCounter(0) {}
		SDescriptorSetEntry(const vk::DescriptorSet& InSetHandle) : 
			LifetimeCounter(0), SetHandle(InSetHandle) {}
	};
	
	/**
	 * Key for a descriptor set entry
	 */
	struct SDescriptorSetEntryKey
	{
		uint32_t Set;
		HandlesArrayType Handles;

		SDescriptorSetEntryKey() : Set(0) {}

		bool operator==(const SDescriptorSetEntryKey& InOther) const
		{
			return Set == InOther.Set &&
				Handles == InOther.Handles;
		}
	};

	struct SDescriptorSetEntryKeyHash
	{
		uint64_t operator()(const SDescriptorSetEntryKey& InKey) const
		{
			uint64_t Hash = 0;

			HashCombine<uint32_t, robin_hood::hash<uint32_t>>(Hash, InKey.Set);

			for (const auto& Handle : InKey.Handles)
			{
				HashCombine<uint64_t, robin_hood::hash<uint64_t>>(Hash, Handle);
			}

			return Hash;
		}
	};
public:
	CVulkanDescriptorSetManager(CVulkanDevice& InDevice,
		CVulkanPipelineLayout& InPipelineLayout);
	~CVulkanDescriptorSetManager();

	void NewFrame();

	/**
	 * Get a descriptor set from the specified Set
	 * Return true if the set needs a update (recycled or allocated)
	 */
	std::pair<vk::DescriptorSet, bool> GetSet(const uint32_t& InSet,
		const std::array<uint64_t, GMaxBindingsPerSet>& InHandles);
private:
	SDescriptorPoolEntry* CreatePool();
private:
	CVulkanDevice& Device;
	CVulkanPipelineLayout& PipelineLayout;

	/** Used descriptor sets */
	robin_hood::unordered_map<SDescriptorSetEntryKey, SDescriptorSetEntry, SDescriptorSetEntryKeyHash> Sets;

	/** Available descriptor sets that have been recycled */
	robin_hood::unordered_map<uint32_t, std::queue<vk::DescriptorSet>> AvailableDescriptorSets;

	/** Pools */
	std::vector<SDescriptorPoolEntry> Pools;
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
	CVulkanDescriptorSetManager& GetSetManager() { return SetManager; }
private:
	CVulkanDescriptorSetManager SetManager;
	std::unordered_map<uint32_t, vk::UniqueDescriptorSetLayout> Layouts;
	vk::UniquePipelineLayout PipelineLayout;
	std::vector<vk::DescriptorPoolSize> PoolSizes;
};