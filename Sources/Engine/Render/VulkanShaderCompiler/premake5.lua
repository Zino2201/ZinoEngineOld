project "VulkanShaderCompiler"
	basicModuleDefinitions()
	includeCoreModules()
	includeVulkan()
	includeModule("Engine/Render/VulkanRenderSystem")
	includeModule("Engine/Render/ShaderCompiler")
	includeModule("Engine/Render/RenderCore")
	includeLib("shaderc/include", "shaderc/lib")
	includeLib("spirv/include", "spirv/lib")
	includeLib("spirv-cross/include", "spirv-cross/lib")
	includeLib("glslang/include", "glslang/lib")
	filter "configurations:Debug"
		links { "shadercd", "shaderc_utild", 
			"spirv-cross-cd", "spirv-cross-cored", "spirv-cross-glsld", "spirv-cross-reflectd", "spirv-cross-utild", "SPIRV-Toolsd",
			"SPIRV-Tools-optd", 
			"glslangd", "SPIRVd", "HLSLd", "OSDependentd", "OGLCompilerd" }
	
	