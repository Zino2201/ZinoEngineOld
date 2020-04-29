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
	includeLib("", "shaderc/build/third_party/spirv-tools/source/opt/Debug")
	includeLib("", "shaderc/build/third_party/spirv-tools/source/Debug")
	includeLib("", "shaderc/build/third_party/glslang/glslang/Debug")
	includeLib("", "shaderc/build/third_party/glslang/OGLCompilersDLL/Debug")
	includeLib("", "shaderc/build/third_party/glslang/SPIRV/Debug")
	includeLib("", "shaderc/build/third_party/glslang/hlsl/Debug")
	includeLib("", "shaderc/build/third_party/glslang/glslang/OSDependent/Windows/Debug")
	filter "configurations:Debug"
		links { "shaderc", "shaderc_util", 
			"spirv-cross-cd", "spirv-cross-cored", "spirv-cross-glsld", "spirv-cross-reflectd", "spirv-cross-utild", "SPIRV-Tools",
			"SPIRV-Tools-opt",
			"glslangd", "SPIRVd", "HLSLd", "OSDependentd", "OGLCompilerd" }
	
	