local Mod = Module:new("VulkanRenderSystem")
Mod:addModules({ "EngineCore", "RenderSystem", "RenderCore", "ShaderCore" })
Mod:addIncludeDirs({ LibDir.."/VulkanMemoryAllocator" })

if type(os.getenv("VULKAN_SDK")) ~= "" then
    local includeDir = "Include"
    local libDir = "Lib"
    local lib = "vulkan-1"

    if os.istarget("linux") then
        includeDir = "include"
        libDir = "lib"
        lib = "vulkan"
    end
    
    Mod:addIncludeDirs({ os.getenv("VULKAN_SDK").."/"..includeDir })
    Mod:addLibDirs({ os.getenv("VULKAN_SDK").."/"..libDir })
    Mod:addLibs({ "vulkan" })
else
    print("Failed to found VulkanSDK, check your installation")
end