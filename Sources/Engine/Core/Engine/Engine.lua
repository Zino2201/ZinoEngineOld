local Mod = Module:new("Engine")
Mod:addModules({ "EngineCore", "RenderCore", "ShaderCore", "RenderSystem", "ShaderCompiler", "Reflection", "ImGui", "Renderer" })
Mod:addIncludeDirs({ LibDir.."/tinyobjloader" })
Mod:addLibs({ "SDL2" })