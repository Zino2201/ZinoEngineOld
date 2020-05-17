SrcDir = os.getcwd()
LibDir = SrcDir.."/Libs/"
BuildDir = SrcDir.."/../Build/"
IntermediateDir = BuildDir.."Intermediates/"
BinDir = SrcDir.."/../Binaries/"

-- Utils for creating a module
function basicModuleDefinitions(consoleApp)
	language "C++"
	cppdialect "C++17"
	if type(consoleApp) == "boolean" then
		kind "ConsoleApp"
	else
		kind "SharedLib"
	end
	files { "**.h", "**.cpp", "**.hpp", "**.cxx" }
	
	defines("%{prj.name:upper()}_API=__declspec(dllexport)")

	targetdir (BinDir.."%{cfg.longname}")
	
	objdir (IntermediateDir.."%{prj.name}/".."%{cfg.longname}")
	
	includedirs "Public"
	includedirs "Private"
	
	exceptionhandling("Off")
	rtti("Off")
	debugdir(SrcDir.."/../")
	-- prebuildcommands { "call \""..SrcDir.."/GenerateReflectionData.bat\" "..os.getcwd().." "..IntermediateDir.."%{prj.name}/".."%{cfg.longname}/Generated"  }
end

-- Include a module
function includeModule(path)
	local explodedPath = string.explode(path, "/")
	local moduleName = explodedPath[#explodedPath]

	includedirs(SrcDir.."/"..path.."/Public")
	libdirs(BinDir.."/%{cfg.longname}")
	dependson(moduleName)
	defines(string.upper(moduleName).."_API=__declspec(dllimport)")
	links("ZinoEngine-"..moduleName)
end

-- Include a module that will be accessed dynamicaly
function includeModuleDynamic(path)
	local explodedPath = string.explode(path, "/")
	local moduleName = explodedPath[#explodedPath]
	
	-- No dependson
	-- No lib linking
	
	includedirs(SrcDir.."/"..path.."/Public")
	defines(string.upper(moduleName).."_API=__declspec(dllimport)")
end

function includeLib(path, libPath)
	includedirs(LibDir..path)
	if type(libPath) == "string" then
		libdirs(LibDir..libPath)
	end
end

function includeMinimalModules()
	includeCoreModules()
	includeRenderModules()
end

function includeCoreModules()
	-- Base Core Engine module
	includeModule("Engine/Core/EngineCore")
	includeModule("Engine/Core/Reflection")
	
	includeCoreLibs()
end

function includeRenderModules()
	includeModule("Engine/Render/Render")
	includeModule("Engine/Render/Renderer")
end

function includeCoreLibs()
	-- Basic libs
	includeLib("glm")
	includeLib("stb_image")
	includeLib("SDL2/include", "SDL2/lib")
	includeLib("boost")
	includeLib("rapidjson-1.1.0/include")
	
	links("SDL2")
end

function includeVulkan()
	includeLib("Vulkan/Include", "Vulkan/Lib")
	includeLib("VulkanMemoryAllocator")
end

workspace "ZinoEngine"
	location ("../Build/ProjectFiles")
	configurations { "Debug", "Release" }
	language "C++"
	architecture "x86_64"
	filter { "configurations:Debug" }
		symbols "On"
	filter { "configurations:Release" }
		optimize "On"
	filter { }
	startproject "Main"
	
	targetprefix ("ZinoEngine-")
	
	include("Engine/Core/Main")
	group "Engine/Core"
		include("Engine/Core/Engine")
		include("Engine/Core/EngineCore")
		include("Engine/Core/Reflection")
	group "Engine/Rendering"
		include("Engine/Render/RenderCore")
		include("Engine/Render/RenderSystem")
		include("Engine/Render/VulkanRenderSystem")
		include("Engine/Render/Renderer")
	group "Engine/Rendering/Shaders"
		include("Engine/Render/Shaders")
		include("Engine/Render/ShaderCore")
		include("Engine/Render/ShaderCompiler")
		include("Engine/Render/VulkanShaderCompiler")
	