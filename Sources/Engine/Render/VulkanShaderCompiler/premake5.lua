project "VulkanShaderCompiler"
	basicModuleDefinitions()
	includeCoreModules()
	includeVulkan()
	includeModule("Engine/Render/ShaderCore")
	includeModule("Engine/Render/VulkanRenderSystem")
	includeModule("Engine/Render/ShaderCompiler")
	includeModule("Engine/Render/RenderCore")
	includeLib("ShaderConductor/Include", "ShaderConductor/Build/vs2019-win-vc150-x64/Lib/Debug/")
	includeLib("ShaderConductor/External/SPIRV-Cross", "")
	filter "configurations:Debug"
		links { "spirv-cross-cored", "spirv-cross-glsld", "SPIRV-Tools",
			"SPIRV-Tools-opt", "ShaderConductor" }
	
	