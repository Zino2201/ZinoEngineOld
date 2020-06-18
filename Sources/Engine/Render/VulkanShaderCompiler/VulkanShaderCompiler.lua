local Mod = Module:new("VulkanShaderCompiler")
Mod:addModules({ "EngineCore", "ShaderCore", "VulkanRenderSystem", "ShaderCompiler", "RenderCore" })
Mod:addIncludeDirs({ LibDir.."ShaderConductor/Include", LibDir.."ShaderConductor/External/SPIRV-Cross" })
Mod:addLibs({ "ShaderConductor" })
filterDebugOnly()
	Mod:addLibDirs({LibDir.."ShaderConductor/Build/vs2019-win-vc142-x64/Lib/Debug/"})
	Mod:addLibs({ "spirv-cross-cored", "SPIRV-Tools",
		"SPIRV-Tools-opt" })
filterReleaseOnly()
	Mod:addLibDirs({LibDir.."ShaderConductor/Build/vs2019-win-vc142-x64/Lib/Release/"})
	Mod:addLibs({"spirv-cross-core", "SPIRV-Tools",
		"SPIRV-Tools-opt"})
filterReset()