local Mod = Module:new("VulkanShaderCompiler")
Mod:addModules({ "EngineCore", "ShaderCore", "VulkanRenderSystem", "ShaderCompiler", "RenderCore" })
Mod:addIncludeDirs({ LibDir.."/ShaderConductor/Include", LibDir.."/ShaderConductor/External/SPIRV-Cross" })
filterDebugOnly()
	if os.istarget("linux") then
		Mod:addLibDirs({LibDir.."/ShaderConductor/Build/ninja-linux-clang-x64-Debug/Lib/"})
		filterModularOnly()
		linkoptions("-Wl,-whole-archive -lSPIRV-Tools -lSPIRV-Tools-opt -lspirv-cross-core -Wl,-no-whole-archive")
	else
		Mod:addLibDirs({LibDir.."/ShaderConductor/Build/vs2019-win-vc142-x64/Lib/Debug"})
		Mod:addLibs({ "SPIRV-Tools",
			"SPIRV-Tools-opt", "ShaderConductor", "spirv-cross-cored" })
	end
filterReleaseOnly()
	if os.istarget("linux") then
		Mod:addLibDirs({LibDir.."/ShaderConductor/Build/ninja-linux-clang-x64-Release/Lib/"})
	else
		Mod:addLibDirs({LibDir.."/ShaderConductor/Build/vs2019-win-vc142-x64/Lib/Release"})
	end
	Mod:addLibs({"spirv-cross-core", "SPIRV-Tools",
	"SPIRV-Tools-opt", "ShaderConductor"})
filterReset()
Mod:addLibs({ "ShaderConductor" })