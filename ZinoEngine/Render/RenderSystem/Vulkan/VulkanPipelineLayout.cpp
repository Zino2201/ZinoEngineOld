#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanSwapChain.h"

CVulkanDescriptorPoolManager::CVulkanDescriptorPoolManager(const uint32_t& InMaxSetCountPerPool,
	const std::vector<vk::DescriptorPoolSize>& InPoolSizes,
	const std::map<uint32_t, vk::UniqueDescriptorSetLayout>& InSetLayouts)
	: MaxSetCountPerPool(InMaxSetCountPerPool), PoolSizes(InPoolSizes)
{
	for(const auto& [Set, SetLayout] : InSetLayouts)
		SetLayouts[Set] = std::vector<vk::DescriptorSetLayout>(
			g_VulkanRenderSystem->GetSwapChain()->GetImageCount(),
			*SetLayout);

	if(!PoolSizes.empty())
		CreatePool();
}

void CVulkanDescriptorPoolManager::CreatePool()
{
	uint32_t Index = 0; // g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame();

	//for(uint32_t i = 0; i < g_VulkanRenderSystem->GetSwapChain()->GetImageCount() - 1; ++i)
	for(uint32_t i = 0; i < g_VulkanRenderSystem->GetSwapChain()->GetImageCount() - 2; ++i)
	{
		DescriptorPools[i].push_back({ 
			g_VulkanRenderSystem->GetDevice()->GetDevice().createDescriptorPoolUnique(
			vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(),
				MaxSetCountPerPool,
				static_cast<uint32_t>(PoolSizes.size()),
				PoolSizes.data())).value, 0 });

		CurrentPoolForFrame[Index] = 
			static_cast<uint32_t>(DescriptorPools[i].size()) - 1;
	}
}

bool CVulkanDescriptorPoolManager::FindFreePool(uint32_t& InIndex)
{
	uint32_t Index = 0; // g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame();

	uint32_t Idx = 0;
	for(const auto& DescriptorPool 
		: DescriptorPools[Index])
	{
		if(DescriptorPool.Allocations < MaxSetCountPerPool)
		{
			InIndex = Idx;
			return true;
		}

		Idx++;
	}

	return false;
}

vk::DescriptorSet CVulkanDescriptorPoolManager::AllocateSet(const uint32_t& InSet)
{
	uint32_t Index = 0; // g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame();

	bool bHasFoundFreePool = 
		FindFreePool(CurrentPoolForFrame[Index]);
	if (!bHasFoundFreePool)
	{
		CreatePool();
	}

	std::vector<vk::DescriptorSetLayout> Layouts = { 
		SetLayouts[InSet][Index] };

	SVulkanDescriptorPoolEntry& PoolToUse = 
		DescriptorPools[Index]
			[CurrentPoolForFrame[Index]];

	std::vector<vk::DescriptorSet> DescriptorSet = 
		g_VulkanRenderSystem->GetDevice()->GetDevice().allocateDescriptorSets(
			vk::DescriptorSetAllocateInfo(
				*PoolToUse.DescriptorPool,
				static_cast<uint32_t>(Layouts.size()),
				Layouts.data())).value;
	if(!DescriptorSet.front())
		LOG(ELogSeverity::Fatal, "Failed to allocate set")

	PoolToUse.Allocations++;

	return DescriptorSet.front();
}

void CVulkanDescriptorPoolManager::ResetPools()
{
	uint32_t CurrentFrame = g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame();
	uint32_t Index = 0; // std::clamp<uint32_t>(CurrentFrame, 0,
		//g_VulkanRenderSystem->GetSwapChain()->GetImageCount() - 1);

	for(auto& DescriptorPool : 
		DescriptorPools[Index])
	{
		g_VulkanRenderSystem->GetDevice()->GetDevice().resetDescriptorPool(
			*DescriptorPool.DescriptorPool);
		DescriptorPool.Allocations = 0;
	}


	CurrentPoolForFrame[Index] = 0;
}

constexpr uint32_t MaxSetPipelineLayout = 32;

std::vector<CVulkanPipelineLayout*> CVulkanPipelineLayout::PipelineLayouts;

CVulkanPipelineLayout::CVulkanPipelineLayout(CVulkanDevice* InDevice,
	const SVulkanPipelineLayoutInfos& InInfos) :
	CVulkanDeviceResource(InDevice)
{
	for (const auto& [Set, Bindings] : InInfos.SetLayoutBindings)
	{
		DescriptorSetLayouts.insert(std::make_pair(Set,
			Device->GetDevice().createDescriptorSetLayoutUnique(
				vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(),
					static_cast<uint32_t>(Bindings.size()),
					Bindings.data())).value));
	}

	std::vector<vk::DescriptorSetLayout> LayoutSetLayouts;
	LayoutSetLayouts.reserve(DescriptorSetLayouts.size());
	for (const auto& [Set, SetLayout] : DescriptorSetLayouts)
		LayoutSetLayouts.push_back(*SetLayout);

	vk::PipelineLayoutCreateInfo CreateInfos(
		vk::PipelineLayoutCreateFlags(),
		static_cast<uint32_t>(LayoutSetLayouts.size()),
		LayoutSetLayouts.data(),
		0,
		nullptr);

	PipelineLayout = Device->GetDevice().createPipelineLayoutUnique(CreateInfos).value;
	if (!PipelineLayout)
		LOG(ELogSeverity::Fatal, "Failed to create pipeline layout");
		
	std::vector<vk::DescriptorPoolSize> PoolSizes;
	for(const auto& [Set, Bindings] : InInfos.SetLayoutBindings)
	{
		for(const auto& Binding : Bindings)
		{
			PoolSizes.emplace_back(Binding.descriptorType,
				MaxSetPipelineLayout);
		}
	}

	DescriptorPoolManager = std::make_unique<CVulkanDescriptorPoolManager>(MaxSetPipelineLayout,
		PoolSizes,
		DescriptorSetLayouts);

	PipelineLayouts.push_back(this);
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() 
{ 
	PipelineLayouts.erase(std::remove(begin(PipelineLayouts), end(PipelineLayouts), this), 
		end(PipelineLayouts));
}