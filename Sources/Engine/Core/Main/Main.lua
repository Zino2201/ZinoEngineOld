local Mod = Module:new("Main", "WindowedApp")
Mod:addModules({ "EngineCore", "RenderCore", "ShaderCore", "RenderSystem", "VulkanRenderSystem", "ShaderCompiler", "VulkanShaderCompiler", "Engine" })
targetprefix("")
targetname("ZinoEngine")
-- Link all modules if in monolithic mode
for k, v in pairs(Modules) do
	if v.kind == "SharedLib" then
		filterMonolithicOnly()
			links("ZinoEngine-"..v.name)
			dependson(v.name)
			linkoptions("/WHOLEARCHIVE:ZinoEngine-"..v.name..".lib")
			
			for k, v in pairs(v.bothLibDirs) do
				libdirs(v)
			end
			
			for k, v in pairs(v.bothLibs) do
				links(v)
			end
		filterDebugMonolithicOnly()
			for k, v in pairs(v.debugLibDirs) do
				libdirs(v)
			end
				
			for k, v in pairs(v.debugLibs) do
				links(v)
			end
		
		filterReleaseMonolithicOnly()
			for k, v in pairs(v.releaseLibDirs) do
				libdirs(v)
			end
				
			for k, v in pairs(v.releaseLibs) do
				links(v)
			end
		filterReset()
	end
end
