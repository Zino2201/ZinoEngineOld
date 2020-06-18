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

/** Cache manager */

CVulkanDescriptorCacheManager::CVulkanDescriptorCacheManager(CVulkanDevice* InDevice, 
	CVulkanPipelineLayout* InLayout,
	const uint32_t& InMaxAllocationsPerPool) : Device(InDevice), 
	Layout(InLayout),
	MaxAllocationsPerPool(InMaxAllocationsPerPool) {}

CVulkanDescriptorCacheManager::~CVulkanDescriptorCacheManager()
{
	for (const auto& Pool : Pools)
	{
		Device->GetDevice().destroyDescriptorPool(Pool.Pool);
	}

	Pools.clear();
}

vk::DescriptorSet CVulkanDescriptorCacheManager::AllocateDescriptorSet(const uint32_t& InSet,
	vk::DescriptorPool& OutPool)
{
	SDescriptorPoolEntry& Pool = GetPool();

	OutPool = Pool.Pool;

	auto& Sets = Device->GetDevice().allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo(
			Pool.Pool,
			1,
			&Layout->GetLayoutForSet(InSet))).value;
	if(!Sets.front())
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to allocate descriptor set");

	LOG(ELogSeverity::Debug, VulkanRS, "new vk::DescriptorSet");

	Pool.Allocations++;

	return Sets.front();
}

CVulkanDescriptorCacheManager::SDescriptorPoolEntry& CVulkanDescriptorCacheManager::GetPool()
{
	for(auto& Entry : Pools)
	{
		if(Entry.Allocations < MaxAllocationsPerPool - 1)
			return Entry;
	}

	/**
	 * No pool found, allocate new one
	 */
	Pools.emplace_back(Device->GetDevice().createDescriptorPool(
		vk::DescriptorPoolCreateInfo(
			vk::DescriptorPoolCreateFlags(),
			MaxAllocationsPerPool,
			static_cast<uint32_t>(Layout->GetPoolSizes().size()),
			Layout->GetPoolSizes().data())).value);

	LOG(ELogSeverity::Debug, VulkanRS, "new vk::DescriptorPool");

	return Pools.back();
}

constexpr uint32_t GMaxAllocationsPerPool = 42;

CVulkanPipelineLayout::CVulkanPipelineLayout(CVulkanDevice* Device,
	const SVulkanPipelineLayoutDesc& InDesc) :
	CVulkanDeviceResource(Device)
{
	std::vector<vk::DescriptorSetLayout> SetLayouts;
	Layouts.reserve(InDesc.SetLayoutBindings.size());
	SetLayouts.reserve(InDesc.SetLayoutBindings.size());

	std::unordered_map<vk::DescriptorType, uint32_t> DescriptorMap;

	{
		uint32_t Set = 0;
		for (const auto& SetBindings : InDesc.SetLayoutBindings)
		{
			vk::DescriptorSetLayoutCreateInfo CreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				static_cast<uint32_t>(SetBindings.Bindings.size()),
				SetBindings.Bindings.data());

			Layouts.insert(std::make_pair(Set, 
				Device->GetDevice().createDescriptorSetLayoutUnique(CreateInfo).value));

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

	LOG(ELogSeverity::Debug, VulkanRS, "new vk::PipelineLayout");

	CacheMgr = std::make_unique<CVulkanDescriptorCacheManager>(Device, 
		this, GMaxAllocationsPerPool);
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() {}