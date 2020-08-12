local Mod = Module:new("Main", "WindowedApp")
Mod:addModules({ "EngineCore", "RenderCore", "ShaderCore", "RenderSystem", "VulkanRenderSystem", "ShaderCompiler", "VulkanShaderCompiler", "Engine", "Editor" })
targetprefix("")
targetname("ZinoEngine")

-- Link all modules if in monolithic mode
for k, v in pairs(Modules) do
	local name = v.name

	if v.kind == "SharedLib" then
		-- Add lib dirs for this module
		-- so ld on gcc/clang don't complain about missing
		-- shared objects dependencies
		if os.istarget("linux") then
			Mod:addLibDirs(v.bothLibDirs)

			filterDebugOnly()
				Mod:addLibDirs(v.debugLibDirs)
			filterReleaseOnly()
				Mod:addLibDirs(v.releaseLibDirs)
		else
			filterMonolithicOnly()
				linkoptions("/WHOLEARCHIVE:ZinoEngine-"..v.name..".lib")
				links(name)
				dependson(v.name)

				for k, v in pairs(v.bothLibs) do
					links(v)
				end
	
				for k, v in pairs(v.bothLibDirs) do
					libdirs(v)
				end
			filterDebugMonolithicOnly()
				for k, v in pairs(v.debugLibs) do
					links(v)
				end
				for k, v in pairs(v.debugLibDirs) do
					libdirs(v)
				end

			filterReleaseMonolithicOnly()
				for k, v in pairs(v.releaseLibs) do
					links(v)
				end

				for k, v in pairs(v.releaseLibDirs) do
					libdirs(v)
				end
			filterReset()
		end
	end
end
