project "Main"
	basicModuleDefinitions(true)
	includeCoreModules()
	includeModule("Engine/Core/Engine")
	includeModule("Engine/Render/RenderCore")
	includeModule("Engine/Render/RenderSystem")
	includeModule("Engine/Render/VulkanRenderSystem")
	includeModule("Engine/Render/ShaderCompiler")
	includeModule("Engine/Render/VulkanShaderCompiler")