local Mod = Module:new("ImGui")
Mod:addModules({ "EngineCore", "RenderCore", "ShaderCore", "ShaderCompiler", "RenderSystem" })
files { LibDir.."/imgui/*.cpp" }
files { LibDir.."/imgui/examples/imgui_impl_sdl.cpp" }