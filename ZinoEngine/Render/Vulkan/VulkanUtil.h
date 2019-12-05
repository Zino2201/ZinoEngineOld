#pragma once

#include "VulkanCore.h"

/** Vulkan util */
namespace VulkanUtil
{
	SVulkanSwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& InDevice,
		const vk::SurfaceKHR& InSurface);
	SVulkanQueueFamilyIndices GetQueueFamilyIndices(const vk::PhysicalDevice& InDevice,
		const vk::SurfaceKHR& InSurface);
	vk::ShaderStageFlagBits ShaderStageToVkShaderStage(const EShaderStage& InShaderStage);
	vk::VertexInputRate VertexInputRateToVkVertexInputRate(const EVertexInputRate& InInputRate);
	vk::Format FormatToVkFormat(const EFormat& InFormat);
	vk::BufferUsageFlags BufferUsageFlagsToVkBufferUsageFlags(const EBufferUsageFlags& InUsage);
	VmaMemoryUsage BufferMemoryUsageToVmaMemoryUsage(const EBufferMemoryUsage& InUsage);
	vk::IndexType IndexFormatToVkIndexType(const EIndexFormat& InFormat);
	vk::DescriptorType ShaderAttributeTypeToVkDescriptorType(const EShaderAttributeType& InType);
	vk::ShaderStageFlags ShaderStageFlagsToVkShaderStageFlags(const EShaderStageFlags& InFlags);
	vk::ImageType TextureTypeToVkImageType(const ETextureType& InType);
	vk::ImageUsageFlags TextureUsageFlagsToVkImageUsageFlags(const ETextureUsageFlags& InUsage);
	VmaMemoryUsage TextureMemoryUsageToVmaMemoryUsage(const ETextureMemoryUsage& InUsage);
	vk::ImageViewType TextureViewTypeViewToVkImageViewType(const ETextureViewType& InViewType);
	vk::Filter SamplerFilterToVkFilter(const ESamplerFilter& InFilter);
	vk::SamplerMipmapMode SamplerFilterToVkSamplerMipmapMode(const ESamplerFilter& InFilter);
	vk::SamplerAddressMode SamplerAddressModeToVkSamplerAddressMode(const ESamplerAddressMode& InMode);
	vk::CompareOp ComparisonOpToVkCompareOp(const EComparisonOp& InOp);
}