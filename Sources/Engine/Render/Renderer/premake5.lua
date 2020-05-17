project "Renderer"
	basicModuleDefinitions()
	includeCoreModules()
	includeModule("Engine/Render/RenderCore")
	includeModule("Engine/Render/RenderSystem")
	includeModule("Engine/Render/ShaderCore")
	includeModule("Engine/Render/ShaderCompiler")