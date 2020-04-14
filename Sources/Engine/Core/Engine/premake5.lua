project "Engine"
	basicModuleDefinitions()
	includeCoreModules()
	includeModule("Engine/Render/RenderCore")
	includeModule("Engine/Render/RenderSystem")
	includeModule("Engine/Render/ShaderCompiler")
	includeModule("Engine/Core/Reflection")
	includeLib("assimp/include", "assimp/lib")
	links("assimp-vc142-mt")