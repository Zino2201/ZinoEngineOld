#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"

CVulkanPipelineLayoutManager::CVulkanPipelineLayoutManager(CVulkanDevice* InDevice) 
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

		Layouts.insert(std::make_pair(InEntry, Layout));

		LOG(ELogSeverity::Debug, VulkanRS, "new CVulkanPipelineLayout");

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
	std::vector<size_t> EntriesToFree;
	EntriesToFree.reserve(10);

	size_t Idx = 0;
	for(auto& Entry : Sets)
	{
		if (Entry.LifetimeCounter + 1 > GMaxLifetimeDescriptorSet)
		{
			EntriesToFree.emplace_back(Idx);
		}
		else
		{
			Entry.LifetimeCounter++;
		}

		Idx++;
	}

	for(const auto& Idx : EntriesToFree)
	{
		const auto& Entry = Sets[Idx];

		AvailableDescriptorSets[Entry.Set].push(std::move(Entry.SetHandle));
		Sets.erase(Sets.begin() + Idx);
	}
}

std::pair<vk::DescriptorSet, bool> CVulkanDescriptorSetManager::GetSet(const uint32_t& InSet,
	const std::array<uint64_t, GMaxBindingsPerSet>& InHandles)
{
	/**
	 * First search if there is already a set with the same handles
	 */

	for(auto& Entry : Sets)
	{
		if (Entry.Set == InSet &&
			Entry.Handles == InHandles)
		{
			Entry.LifetimeCounter = 0;

			return { Entry.SetHandle, false };
		}
	}

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
					LOG(ELogSeverity::Error, VulkanRS, "VulkanDescSetManager: Failed to allocate set");
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
				LOG(ELogSeverity::Error, VulkanRS, "VulkanDescSetManager: Failed to allocate set");
				return { Set, false };
			}
			Pool->Allocations++;
		}
	}

	Sets.emplace_back(InSet, InHandles, Set);

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
		LOG(ELogSeverity::Error, VulkanRS, "VulkanDescSetManager: Failed to create pool");
		return nullptr;
	}

	Pools.emplace_back(SDescriptorPoolEntry(Pool));

	return &Pools.back();
}

/*
 * Vulkan pipeline layout
 */
constexpr uint32_t GMaxAllocationsPerPool = 42;

CVulkanPipelineLayout::CVulkanPipelineLayout(CVulkanDevice* Device,
	const SVulkanPipelineLayoutDesc& InDesc) :
	CVulkanDeviceResource(Device), SetManager(*Device, *this)
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

			Layouts.insert(std::make_pair(Set, Device->GetDevice().createDescriptorSetLayoutUnique(LayoutCreateInfo).value));

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

 	PipelineLayout = Device->GetDevice().createPipelineLayoutUnique(CreateInfo).value;
	if (!PipelineLayout)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create Vulkan pipeline layout");
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() {}