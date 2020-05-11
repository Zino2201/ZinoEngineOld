project "VulkanRenderSystem"
	basicModuleDefinitions()
	includeCoreModules()
	includeVulkan()
	includeModule("Engine/Render/RenderSystem")
	includeModule("Engine/Render/RenderCore")
	includeModule("Engine/Render/ShaderCore")
	links("vulkan-1")