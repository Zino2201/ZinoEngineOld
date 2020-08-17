local Mod = Module:new("EngineCore")
dependson("ZinoReflectionTool")

if os.istarget("linux") then
    filterMonolithicOnly()
    Mod:addLibs({ "dl", "pthread" })
end
