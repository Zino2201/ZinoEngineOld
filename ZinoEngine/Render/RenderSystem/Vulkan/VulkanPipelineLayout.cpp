#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanSwapChain.h"

CVulkanDescriptorPoolManager::CVulkanDescriptorPoolManager(const uint32_t& InMaxSetCountPerPool,
	const std::vector<vk::DescriptorPoolSize>& InPoolSizes,
	const std::map<uint32_t, vk::DescriptorSetLayout>& InSetLayouts)
	: MaxSetCountPerPool(InMaxSetCountPerPool), PoolSizes(InPoolSizes)
{
	for(const auto& [Set, SetLayout] : InSetLayouts)
		SetLayouts[Set] = std::vector<vk::DescriptorSetLayout>(
			g_VulkanRenderSystem->GetSwapChain()->GetImageCount(),
			SetLayout);

	if(!PoolSizes.empty())
		CreatePool();
}

void CVulkanDescriptorPoolManager::CreatePool()
{
	for(int i = 0; i < g_VulkanRenderSystem->GetSwapChain()->GetImageCount() - 1; ++i)
	{
		DescriptorPools[i].push_back({ 
			g_VulkanRenderSystem->GetDevice()->GetDevice().createDescriptorPoolUnique(
			vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(),
				MaxSetCountPerPool,
				static_cast<uint32_t>(PoolSizes.size()),
				PoolSizes.data())).value, 0 });

		CurrentPoolForFrame[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()] = 
			DescriptorPools[i].size() - 1;
	}
}

bool CVulkanDescriptorPoolManager::FindFreePool(uint32_t& InIndex)
{
	uint32_t Idx = 0;
	for(const auto& DescriptorPool 
		: DescriptorPools[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()])
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
	bool bHasFoundFreePool = 
		FindFreePool(CurrentPoolForFrame[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()]);
	if (!bHasFoundFreePool)
	{
		CreatePool();
	}

	std::vector<vk::DescriptorSetLayout> Layouts = { 
		SetLayouts[InSet][g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()] };

	SVulkanDescriptorPoolEntry& PoolToUse = 
		DescriptorPools[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()]
			[CurrentPoolForFrame[g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame()]];

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
	/** If we do -, uint32_t will going to be very high so clamp can work */
	uint32_t CurrentFrame = g_VulkanRenderSystem->GetSwapChain()->GetCurrentFrame();
	uint32_t Index = std::clamp<uint32_t>(CurrentFrame - 1, 0,
		g_VulkanRenderSystem->GetSwapChain()->GetImageCount() - 2);

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
	std::vector<vk::DescriptorSetLayout> LayoutSetLayouts;
	LayoutSetLayouts.reserve(InInfos.SetLayouts.size());
	for (const auto& [Set, SetLayout] : InInfos.SetLayouts)
		LayoutSetLayouts.push_back(SetLayout);

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
	for(const auto& DescriptorSetLayoutBinding : InInfos.SetLayoutBindings)
	{
		PoolSizes.emplace_back(DescriptorSetLayoutBinding.descriptorType,
			MaxSetPipelineLayout);
	}

	DescriptorPoolManager = std::make_unique<CVulkanDescriptorPoolManager>(MaxSetPipelineLayout,
		PoolSizes,
		InInfos.SetLayouts);

	PipelineLayouts.push_back(this);
}

CVulkanPipelineLayout::~CVulkanPipelineLayout() 
{ 
	PipelineLayouts.erase(std::remove(begin(PipelineLayouts), end(PipelineLayouts), this), 
		end(PipelineLayouts));
}