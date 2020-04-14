project "VulkanRenderSystem"
	basicModuleDefinitions()
	includeCoreModules()
	includeVulkan()
	includeModule("Engine/Render/RenderSystem")
	includeModule("Engine/Render/RenderCore")
	links("vulkan-1")