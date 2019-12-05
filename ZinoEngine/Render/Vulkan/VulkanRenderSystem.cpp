#include "VulkanRenderSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include "Core/Engine.h"
#include "Render/Window.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderCommandContext.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "Render/Renderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanCommandBufferManager.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanSampler.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"

CVulkanRenderSystem* g_VulkanRenderSystem = nullptr;

/**
 * Validation layers callback
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT InSeverity,
	VkDebugUtilsMessageTypeFlagsEXT InMessageType,
	const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData,
	void* InUserData)
{
	switch (InSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		LOG(ELogSeverity::Debug, InCallbackData->pMessage)
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LOG(ELogSeverity::Warn, InCallbackData->pMessage)
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LOG(ELogSeverity::Error, InCallbackData->pMessage)
		__debugbreak();
		break;
	}

	return VK_FALSE;
}

vk::Result CreateDebugUtilsMessengerEXT(
	vk::Instance instance,
	const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const vk::AllocationCallbacks* pAllocator,
	vk::DebugUtilsMessengerEXT* pDebugMessenger)
{

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return static_cast<vk::Result>(func(
			static_cast<VkInstance>(instance),
			reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(pCreateInfo),
			reinterpret_cast<const VkAllocationCallbacks*>(pAllocator),
			reinterpret_cast<VkDebugUtilsMessengerEXT*>(pDebugMessenger)));
	}
	else
	{
		return vk::Result::eErrorExtensionNotPresent;
	}
}

void DestroyDebugUtilsMessengerEXT(
	vk::Instance instance,
	vk::DebugUtilsMessengerEXT  debugMessenger,
	const vk::AllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(
			static_cast<VkInstance>(instance),
			static_cast<VkDebugUtilsMessengerEXT>(debugMessenger),
			reinterpret_cast<const VkAllocationCallbacks*>(pAllocator));
	}
}

CVulkanRenderSystem::CVulkanRenderSystem() {}

CVulkanRenderSystem::~CVulkanRenderSystem() 
{
	if(g_VulkanEnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(*Instance, Callback, nullptr);
}

void CVulkanRenderSystem::Initialize()
{
	g_VulkanRenderSystem = reinterpret_cast<CVulkanRenderSystem*>(CEngine::Get().GetRenderSystem());

	/** First check if validation layers are enabled, and are they supported */
	if (g_VulkanEnableValidationLayers && !IsRequiredLayersSupported())
		LOG(ELogSeverity::Fatal, "Required layers not supported")

	/** Get window */
	CWindow* Window = CEngine::Get().GetWindow();

	/** Create Vulkan instance */
	{
		vk::ApplicationInfo ApplicationInfos;

		/** Get required extensions */
		std::vector<const char*> RequiredExtensions = GetRequiredExtensions(Window->GetSDLWindow());

		/** Get supported extensions */
		std::vector<vk::ExtensionProperties> SupportedExtensions =
			vk::enumerateInstanceExtensionProperties().value;

		/** Check if required extensions are supported */
		LOG(ELogSeverity::Debug, "--- Extensions ---")
		int FoundExtensionCount = 0;
		for (const char* RequiredExtension : RequiredExtensions)
		{
			bool Found = false;

			for (const vk::ExtensionProperties& Extension : SupportedExtensions)
			{
				if (FoundExtensionCount == 0)
					LOG(ELogSeverity::Debug, "- %s", Extension.extensionName)

					if (strcmp(Extension.extensionName, RequiredExtension) == 0)
					{
						Found = true;
						FoundExtensionCount++;
						break;
					}
			}

			if (!Found)
				break;
		}

		if (FoundExtensionCount < RequiredExtensions.size())
			LOG(ELogSeverity::Fatal,
				"This system doesn't support required extensions (%d found, %d required)",
				FoundExtensionCount,
				RequiredExtensions.size())
		else
			LOG(ELogSeverity::Info,
				"Found all required extensions!")

			vk::InstanceCreateInfo CreateInfos(
				vk::InstanceCreateFlags(),
				&ApplicationInfos,
				g_VulkanEnableValidationLayers ? static_cast<uint32_t>(g_VulkanValidationLayers.size()) : 0,
				g_VulkanEnableValidationLayers ? g_VulkanValidationLayers.data() : 0,
				static_cast<uint32_t>(RequiredExtensions.size()),
				RequiredExtensions.data());
		Instance = vk::createInstanceUnique(CreateInfos).value;
		if (!Instance)
			LOG(ELogSeverity::Fatal, "Failed to create Vulkan instance")
	}

	/** Create debug callback */
	if (g_VulkanEnableValidationLayers)
	{
		vk::DebugUtilsMessengerCreateInfoEXT CreateInfos(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
			VkDebugCallback);

		CreateDebugUtilsMessengerEXT(*Instance,
			&CreateInfos, nullptr, &Callback);
	}

	/** Create surface */
	{
		VkSurfaceKHR SurfaceTmp;

		if (!SDL_Vulkan_CreateSurface(Window->GetSDLWindow(),
			static_cast<VkInstance>(*Instance),
			&SurfaceTmp))
			LOG(ELogSeverity::Fatal, "Failed to create surface")

		Surface = vk::UniqueSurfaceKHR(SurfaceTmp, *Instance);
	}

	/** Select a physical device and create a logical device */
	{
		std::vector<vk::PhysicalDevice> PhysicalDevices = Instance->enumeratePhysicalDevices().value;
		vk::PhysicalDevice PhysicalDevice;

		for (const vk::PhysicalDevice& PhysDevice : PhysicalDevices)
		{
			if (IsDeviceUseable(PhysDevice))
			{
				PhysicalDevice = PhysDevice;
				break;
			}
		}

		if (PhysicalDevice == VK_NULL_HANDLE)
			LOG(ELogSeverity::Fatal, "Can't found an compatible GPU")

		Device = std::make_unique<CVulkanDevice>(PhysicalDevice);
	}

	/** Create swap chain */
	SwapChain = std::make_unique<CVulkanSwapChain>();
	SwapChain->OnSwapChainRecreated.Bind(
		std::bind(&CVulkanRenderSystem::OnSwapchainRecreated, this));

	CreateSwapchainObjects();

	/** Command context */
	RenderCommandContext = std::make_unique<CVulkanRenderCommandContext>(Device.get(),
		Device->GetGraphicsQueue());

	MemoryPool = std::make_unique<CVulkanCommandPool>(
		Device.get(),
		0,
		vk::CommandPoolCreateFlagBits::eTransient);
}

void CVulkanRenderSystem::OnSwapchainRecreated()
{
	/** Free swapchain objects */
	Framebuffers.clear();
	RenderPass.reset();
	DepthBuffer.reset();
	DepthBufferView.reset();

	/** Recreate it */
	CreateSwapchainObjects();
}

void CVulkanRenderSystem::CreateSwapchainObjects()
{
	vk::Format DepthFormat = FindDepthFormat(Device->GetPhysicalDevice());

	/** Depth buffer image */
	{
		DepthBuffer = std::make_shared<CVulkanTexture>(Device.get(),
			STextureInfo(
				ETextureType::Texture2D,
				VulkanUtil::VkFormatToFormat(DepthFormat),
				ETextureUsage::DepthStencil,
				ETextureMemoryUsage::GpuOnly,
				SwapChain->GetExtent().width,
				SwapChain->GetExtent().height));
		DepthBufferView = std::make_shared<CVulkanTextureView>(Device.get(),
			STextureViewInfo(
				DepthBuffer.get(),
				ETextureViewType::DepthStencil,
				VulkanUtil::VkFormatToFormat(DepthFormat)));
	}

	/** Create render pass */
	{
		vk::AttachmentDescription ColorAttachment(
			vk::AttachmentDescriptionFlags(),
			SwapChain->GetSurfaceFormat().format,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR);

		vk::AttachmentDescription DepthAttachment(
			vk::AttachmentDescriptionFlags(),
			DepthFormat,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eDontCare,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::AttachmentReference ColorAttachmentRef(
			0,
			vk::ImageLayout::eColorAttachmentOptimal);


		vk::AttachmentReference DepthAttachmentRef(
			1,
			vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::SubpassDescription Subpass(
			vk::SubpassDescriptionFlags(),
			vk::PipelineBindPoint::eGraphics,
			0,
			nullptr,
			1,
			&ColorAttachmentRef,
			nullptr,
			&DepthAttachmentRef);

		vk::SubpassDependency SubpassDependency(
			VK_SUBPASS_EXTERNAL,
			0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlags(),
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

		std::array<vk::AttachmentDescription, 2> Attachments = { ColorAttachment, DepthAttachment };

		vk::RenderPassCreateInfo CreateInfos(
			vk::RenderPassCreateFlags(),
			static_cast<uint32_t>(Attachments.size()),
			Attachments.data(),
			1,
			&Subpass,
			1,
			&SubpassDependency);

		RenderPass = Device->GetDevice().createRenderPassUnique(CreateInfos).value;
		if(!RenderPass)
			LOG(ELogSeverity::Fatal, "Failed to create render pass")
	}

	/** Create framebuffers */
	{
		Framebuffers.resize(SwapChain->GetImageViews().size());

		for (int i = 0; i < SwapChain->GetImageViews().size(); ++i)
		{
			std::array<vk::ImageView, 2> Attachments = 
			{ 
				*SwapChain->GetImageViews()[i], 
				DepthBufferView->GetImageView() 
			}; 

			vk::FramebufferCreateInfo CreateInfo(
				vk::FramebufferCreateFlags(),
				*RenderPass,
				static_cast<uint32_t>(Attachments.size()),
				Attachments.data(),
				SwapChain->GetExtent().width,
				SwapChain->GetExtent().height,
				1);
			
			Framebuffers[i] = Device->GetDevice().createFramebufferUnique(CreateInfo).value;
		}
	}
}

void CVulkanRenderSystem::Prepare()
{
	SwapChain->AcquireImage();

	CVulkanCommandBuffer* CmdBuffer = RenderCommandContext->GetCommandBufferManager()
		->GetMainCommandBuffer();
	CmdBuffer->AddWaitSemaphore(vk::PipelineStageFlagBits::eColorAttachmentOutput,
		SwapChain->GetImageAvailableSemaphore());
}

void CVulkanRenderSystem::Present()
{
	RenderCommandContext->GetCommandBufferManager()
		->SubmitMainCommandBuffer(SwapChain->GetRenderFinishedSemaphore());

	SwapChain->Present(
		Device->GetPresentQueue());
}

void CVulkanRenderSystem::WaitGPU()
{
	/** Just wait for GPU to be done before destroying resources */
	Device->GetDevice().waitIdle();
}

std::shared_ptr<IShader> CVulkanRenderSystem::CreateShader(const std::vector<uint8_t>& InData,
	const EShaderStage& InShaderStage)
{
	return std::make_shared<CVulkanShader>(Device.get(), InData, InShaderStage);
}

std::shared_ptr<IGraphicsPipeline> CVulkanRenderSystem::CreateGraphicsPipeline(const SGraphicsPipelineInfos& InInfos)
{
	return std::make_shared<CVulkanGraphicsPipeline>(Device.get(), InInfos);
}

std::shared_ptr<IBuffer> CVulkanRenderSystem::CreateBuffer(const SBufferInfos& InInfos)
{
	return std::make_shared<CVulkanBuffer>(Device.get(),
		InInfos);
}

std::shared_ptr<IUniformBuffer> CVulkanRenderSystem::CreateUniformBuffer(const SUniformBufferInfos& InInfos)
{
	return std::make_shared<CVulkanUniformBuffer>(Device.get(),
		InInfos);
}

std::shared_ptr<ITexture> CVulkanRenderSystem::CreateTexture(const STextureInfo& InInfos)
{
	return std::make_shared<CVulkanTexture>(Device.get(),
		InInfos);
}

std::shared_ptr<ITextureView> CVulkanRenderSystem::CreateTextureView(const STextureViewInfo& InInfos)
{
	return std::make_shared<CVulkanTextureView>(Device.get(),
		InInfos);
}

std::shared_ptr<ISampler> CVulkanRenderSystem::CreateSampler(const SSamplerInfo& InInfos)
{
	return std::make_shared<CVulkanSampler>(Device.get(),
		InInfos);
}

std::vector<const char*> CVulkanRenderSystem::GetRequiredExtensions(SDL_Window* InWindow) const
{
	uint32_t RequiredExtensionCount;
	SDL_Vulkan_GetInstanceExtensions(InWindow, &RequiredExtensionCount,
		nullptr);

	std::vector<const char*> RequiredExtensions(RequiredExtensionCount);
	SDL_Vulkan_GetInstanceExtensions(InWindow, &RequiredExtensionCount,
		RequiredExtensions.data());

	if (g_VulkanEnableValidationLayers)
		RequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return RequiredExtensions;
}

bool CVulkanRenderSystem::IsRequiredLayersSupported() const
{
	std::vector<vk::LayerProperties> Layers = vk::enumerateInstanceLayerProperties().value;

	for (const char* RequiredLayer : g_VulkanValidationLayers)
	{
		bool Found = false;

		for (const vk::LayerProperties& Layer : Layers)
		{
			if (strcmp(RequiredLayer, Layer.layerName) == 0)
			{
				Found = true;
				break;
			}
		}

		if (!Found)
			return false;
	}

	return true;
}

bool CVulkanRenderSystem::IsDeviceUseable(const vk::PhysicalDevice& InDevice) const
{
	/** Check required extensions */
	std::vector<vk::ExtensionProperties> AvailableExtensions =
		InDevice.enumerateDeviceExtensionProperties().value;

	std::set<std::string> RequiredExtensions(g_VulkanRequiredDeviceExtensions.begin(),
		g_VulkanRequiredDeviceExtensions.end());

	for (const vk::ExtensionProperties& Extension : AvailableExtensions)
		RequiredExtensions.erase(Extension.extensionName);

	bool SwapChainSupported = false;
	SVulkanSwapChainSupportDetails SwapChainDetails = VulkanUtil::QuerySwapChainSupport(InDevice,
		*Surface);
	SwapChainSupported = !SwapChainDetails.Formats.empty() && !SwapChainDetails.PresentModes.empty();

	return RequiredExtensions.empty() && SwapChainSupported;
}

vk::Format CVulkanRenderSystem::FindSupportedFormat(const vk::PhysicalDevice& InDevice,
	const std::vector<vk::Format>& InCandidates,
	vk::ImageTiling InTiling, vk::FormatFeatureFlags InFeatures) const
{
	for(const vk::Format& Format : InCandidates)
	{
		vk::FormatProperties Properties = InDevice.getFormatProperties(Format);

		if(InTiling == vk::ImageTiling::eLinear &&
			(Properties.linearTilingFeatures & InFeatures) == InFeatures)
			return Format;

		if (InTiling == vk::ImageTiling::eOptimal &&
			(Properties.optimalTilingFeatures & InFeatures) == InFeatures)
			return Format;
	}
}

vk::Format CVulkanRenderSystem::FindDepthFormat(const vk::PhysicalDevice& InDevice) const
{
	return FindSupportedFormat(InDevice,
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool CVulkanRenderSystem::HasStencilComponent(vk::Format InFormat) const
{
	return InFormat == vk::Format::eD32SfloatS8Uint || InFormat == vk::Format::eD24UnormS8Uint;
}