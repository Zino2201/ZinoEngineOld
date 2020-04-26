project "VulkanShaderCompiler"
	basicModuleDefinitions()
	includeCoreModules()
	includeVulkan()
	includeModule("Engine/Render/VulkanRenderSystem")
	includeModule("Engine/Render/ShaderCompiler")
	includeModule("Engine/Render/RenderCore")
	includeLib("shaderc/libshaderc/include", "shaderc/build/libshaderc/Debug")
	includeLib("shaderc/libshaderc_util/include", "shaderc/build/libshaderc_util/Debug")
	--includeLib("spirv/include", "spirv/lib")
	includeLib("SPIRV-Cross", "SPIRV-Cross/build/Debug")
	includeLib("", "SPIRV-Tools/build/source/opt/Debug")
	includeLib("", "SPIRV-Tools/build/source/Debug")
	includeLib("", "glslang/build/glslang/Debug")
	includeLib("", "glslang/build/OGLCompilersDLL/Debug")
	includeLib("", "glslang/build/SPIRV/Debug")
	includeLib("", "glslang/build/hlsl/Debug")
	includeLib("", "glslang/build/glslang/OSDependent/Windows/Debug")
	filter "configurations:Debug"
		links { "shaderc", "shaderc_util", 
			"spirv-cross-cd", "spirv-cross-cored", "spirv-cross-glsld", "spirv-cross-reflectd", "spirv-cross-utild", "SPIRV-Tools",
			"SPIRV-Tools-opt",
			"glslangd", "SPIRVd", "HLSLd", "OSDependentd", "OGLCompilerd" }
	
	