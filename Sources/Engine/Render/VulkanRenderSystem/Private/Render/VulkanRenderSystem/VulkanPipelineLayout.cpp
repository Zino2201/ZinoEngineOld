#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"

CVulkanPipelineLayoutManager::CVulkanPipelineLayoutManager(CVulkanDevice& InDevice) 
	: Device(InDevice) { }

CVulkanPipelineLayoutManager::~CVulkanPipelineLayoutManager() { }

CVulkanPipelineLayout* CVulkanPipelineLayoutManager::GetPipelineLayout(
	const SVulkanPipelineLayoutDesc& InEntry)
{
	auto& Result = Layouts.find(InEntry);
	if (Result != Layouts.end())
	{
		return Result->second.get();
	}
	else
	{
		CVulkanPipelineLayout* Layout = new CVulkanPipelineLayout(Device,
			InEntry);

		Layouts.insert( { InEntry, Layout });

		return Layout;
	}
}

/**
 * Vulkan descriptor set manager
 */
CVulkanDescriptorSetManager::CVulkanDescriptorSetManager(CVulkanDevice& InDevice,
	CVulkanPipelineLayout& InPipelineLayout)
	: Device(InDevice), PipelineLayout(InPipelineLayout)
{

}

CVulkanDescriptorSetManager::~CVulkanDescriptorSetManager()
{
	for(const auto& Pool : Pools)
		Device.GetDevice().destroyDescriptorPool(Pool.Pool);
}

void CVulkanDescriptorSetManager::NewFrame()
{
	auto It = Sets.begin();
	while(It != Sets.end())
	{
		if(It->second.LifetimeCounter + 1 > GMaxLifetimeDescriptorSet)
		{
			AvailableDescriptorSets[It->first.Set].push(std::move(It->second.SetHandle));
			It = Sets.erase(It);
		}
		else
		{
			It->second.LifetimeCounter++;
			++It;
		}
	}
}

std::pair<vk::DescriptorSet, bool> CVulkanDescriptorSetManager::GetSet(const uint32_t& InSet,
	const std::array<uint64_t, GMaxBindingsPerSet>& InHandles)
{
	/**
	 * First search if there is already a set with the same handles
	 */

	/** Key to search or add */
	SDescriptorSetEntryKey Key;
	Key.Handles = std::move(InHandles);
	Key.Set = InSet;

	auto& PossibleSet = Sets.find(Key);
	if(PossibleSet != Sets.end())
		return { PossibleSet->second.SetHandle, false };

	vk::DescriptorSet Set;

	/**
	 * If not, try to recycle a descriptor set or allocate a new one
	 * At this point, the set must be updated using vkUpdateDescriptorSets
	 */
	if(!AvailableDescriptorSets[InSet].empty())
	{
		Set = AvailableDescriptorSets[InSet].front();
		AvailableDescriptorSets[InSet].pop();
	}
	else
	{
		/**
		 * Search a free pool and allocate from it
		 */
		bool bHasFoundFreePool = false;
		for(auto& Pool : Pools)
		{
			if(Pool.Allocations < GMaxDescriptorSetPerPool)
			{
				Set = Device.GetDevice().allocateDescriptorSets(
					vk::DescriptorSetAllocateInfo(
						Pool.Pool,
						1,
						&PipelineLayout.GetLayoutForSet(InSet))).value.front();
				if(!Set)
				{
					ZE::Logger::Fatal("Failed to allocate descriptor set for set {}", InSet);
					return { Set, false };
				}
				Pool.Allocations++;

				bHasFoundFreePool = true;

				break;
			}
		}

		if(!bHasFoundFreePool)
		{
			SDescriptorPoolEntry* Pool = CreatePool();
			if(!Pool)
				return { Set, false };

			Set = Device.GetDevice().allocateDescriptorSets(
				vk::DescriptorSetAllocateInfo(
					Pool->Pool,
					1,
					&PipelineLayout.GetLayoutForSet(InSet))).value.front();
			if (!Set)
			{
				ZE::Logger::Error("Failed to allocate descriptor set for set {}", InSet);
				return { Set, false };
			}
			Pool->Allocations++;
		}
	}

	Sets.insert({ std::move(Key), { Set } });

	return { Set, true };
}

CVulkanDescriptorSetManager::SDescriptorPoolEntry* CVulkanDescriptorSetManager::CreatePool()
{
	vk::DescriptorPool Pool = Device.GetDevice().createDescriptorPool(
		vk::DescriptorPoolCreateInfo(
			vk::DescriptorPoolCreateFlags(),
			GMaxDescriptorSetPerPool,
			static_cast<uint32_t>(PipelineLayout.GetPoolSizes().size()),
			PipelineLayout.GetPoolSizes().data())).value;

	if(!Pool)
	{
		ZE::Logger::Error("Failed to create descriptor pool");
		return nullptr;
	}

	Pools.emplace_back(SDescriptorPoolEntry(Pool));

	return &Pools.back();
}

/*
 * Vulkan pipeline layout
 */
constexpr uint32_t GMaxAllocationsPerPool = 42;

CVulkanPipelineLayout::CVulkanPipelineLayout(CVulkanDevice& Device,
	const SVulkanPipelineLayoutDesc& InDesc) :
	CVulkanDeviceResource(Device), SetManager(Device, *this)
{
	std::vector<vk::DescriptorSetLayout> SetLayouts;
	Layouts.reserve(InDesc.SetLayoutBindings.size());
	SetLayouts.reserve(InDesc.SetLayoutBindings.size());

	std::unordered_map<vk::DescriptorType, uint32_t> DescriptorMap;

	{
		uint32_t Set = 0;
		for (const auto& SetBindings : InDesc.SetLayoutBindings)
		{
			vk::DescriptorSetLayoutCreateInfo LayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				static_cast<uint32_t>(SetBindings.Bindings.size()),
				SetBindings.Bindings.data());

			Layouts.insert( { Set, Device.GetDevice().createDescriptorSetLayoutUnique(LayoutCreateInfo).value });

			SetLayouts.push_back(*Layouts[Set]);

			for(const auto& Binding : SetBindings.Bindings)
			{
				DescriptorMap[Binding.descriptorType] += Binding.descriptorCount;
			}

			Set++;
		}
	}

	/** Pool sizes */
	for(const auto& [Type, Count] : DescriptorMap)
	{
		PoolSizes.emplace_back(Type, Count);
	}

	/** Create pipeline layout */
	vk::PipelineLayoutCreateInfo CreateInfo = vk::PipelineLayoutCreateInfo(
		vk::PipelineLayoutCreateFlags(),
		static_cast<uint32_t>(Layouts.size()),
		SetLayouts.data());

 	PipelineLayout = Device.GetDevice().createPipelineLayoutUnique(CreateInfo).value;
	if (!PipelineLayout)
		ZE::Logger::Fatal("Failed to create Vulkan pipeline layout");
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() {}