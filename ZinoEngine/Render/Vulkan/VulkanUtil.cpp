#include "VulkanUtil.h"

SVulkanSwapChainSupportDetails VulkanUtil::QuerySwapChainSupport(const vk::PhysicalDevice& InDevice,
	const vk::SurfaceKHR& InSurface)
{
	SVulkanSwapChainSupportDetails Details;

	Details.Capabilities = InDevice.getSurfaceCapabilitiesKHR(InSurface);
	Details.Formats = InDevice.getSurfaceFormatsKHR(InSurface);
	Details.PresentModes = InDevice.getSurfacePresentModesKHR(InSurface);

	return Details;
}

SVulkanQueueFamilyIndices VulkanUtil::GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice,
	const vk::SurfaceKHR& InSurface)
{
	SVulkanQueueFamilyIndices Indices;

	std::vector<vk::QueueFamilyProperties> QueueFamilies = InDevice.getQueueFamilyProperties();

	int i = 0;
	for (const vk::QueueFamilyProperties& QueueFamily : QueueFamilies)
	{
		if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			Indices.Graphics = i;

		if (InDevice.getSurfaceSupportKHR(i, InSurface))
			Indices.Present = i;

		if (Indices.IsComplete())
			break;

		i++;
	}

	return Indices;
}

vk::ShaderStageFlagBits VulkanUtil::ShaderStageToVkShaderStage(const EShaderStage& InShaderStage)
{
	switch (InShaderStage)
	{
	default:
	case EShaderStage::Vertex:
		return vk::ShaderStageFlagBits::eVertex;
	case EShaderStage::Fragment:
		return vk::ShaderStageFlagBits::eFragment;
	}
}