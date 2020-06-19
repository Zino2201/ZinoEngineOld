-- Useful globals
SrcDir = os.getcwd()
LibDir = SrcDir.."/Libs/"
BuildDir = SrcDir.."/../Build/"
IntermediateDir = BuildDir.."Intermediates/"
BinDir = SrcDir.."/../Binaries/"

-- Src engine path
SrcEnginePaths = os.matchdirs(SrcDir.."/Engine/**")
SrcEnginePathStr = ""
for k, v in pairs(SrcEnginePaths) do
	SrcEnginePathStr = SrcEnginePathStr..v..";"
end

-- List of modules
Modules = {}

-- Module class
Module = {}
Module.__index = Module

-- Required libs
RequiredIncludes = 
	{ 
		LibDir.."glm",
		LibDir.."stb_image",
		LibDir.."SDL2/include",
		LibDir.."boost"
	}
RequiredLibDirs = 
	{
		LibDir.."SDL2/lib"
	}
RequiredLibs = 
	{
		"SDL2"
	}

currentFilter = -1

-- Utils functions
function filterDebugOnly()
	filter { "Debug or Debug Monolithic" }
	
	currentFilter = 0
end

function filterReleaseOnly()
	filter { "Release or Release Editor or Release Monolithic" }
	
	currentFilter = 1
end

function filterMonolithicOnly()
	filter { "Debug Monolithic or Release Monolithic" }
	
	currentFilter = 2
end

function filterModularOnly()
	filter { "Debug or Release or Release Editor" }
	
	currentFilter = 3
end

function filterDebugModularOnly()
	filter { "Debug or Debug Editor" }
	
	currentFilter = 4
end

function filterReleaseModularOnly()
	filter { "Release or Release Editor" }
	
	currentFilter = 5
end

function filterDebugMonolithicOnly()
	filter { "Debug Monolithic" }
	
	currentFilter = 6
end

function filterReleaseMonolithicOnly()
	filter { "Release Monolithic" }
	
	currentFilter = 7
end

function filterReset()
	filter {}
	
	currentFilter = -1
end

function filterApplyByIdx(idx)
	if idx == 0 then
		filterDebugOnly()
	elseif idx == 1 then
		filterReleaseOnly()
	elseif idx == 2 then
		filterMonolithicOnly()
	elseif idx == 3 then
		filterModularOnly()
	elseif idx == 4 then
		filterDebugModularOnly()
	elseif idx == 5 then
		filterReleaseModularOnly()
	elseif idx == 6 then
		filterDebugMonolithicOnly()
	elseif idx == 7 then
		filterReleaseMonolithicOnly()
	else
		filterReset()
	end 
end

function appendTables(t1, t2) 
	local t1size = #t1
	for k, v in pairs(t2) do 
		t1[t1size + k] = v 
	end
end

-- Declare a module

local currentModule = nil

function Module:new(name, modKind)
	-- Initialize module class
	local mod = {}
	setmetatable(mod, Module)
	-- Set values
	mod.name = name
	mod.kind = modKind or "SharedLib"
	mod.deps = {}
	mod.includeDirs = {}
	mod.libDirs = {}
	mod.libs = {}
	mod.publicHeaders = {}
	mod.root = os.getcwd()
	
	-- Used by monolithic builds
	mod.debugLibDirs = {}
	mod.debugLibs = {}
	mod.releaseLibDirs = {}
	mod.releaseLibs = {}
	mod.bothLibDirs = {}
	mod.bothLibs = {}
	
	-- Initialize project
	
	-- Base configuration
	project(name)
	
	kind(mod.kind)
	
	filterMonolithicOnly()
		if mod.kind == "SharedLib" then
			kind("StaticLib")
		else
			kind(mod.kind)
		end
		defines(string.upper(mod.name).."_API=")
		defines("ZE_MONOLITHIC")
	filterModularOnly()
		defines(string.upper(mod.name).."_API=__declspec(dllexport)")
	filterReset()
	
	-- Add a define for each configurations
	filter "Debug"
		defines("ZE_CONFIGURATION_NAME=\"Debug\"")
	filter "Debug Editor"
		defines("ZE_CONFIGURATION_NAME=\"Debug Editor\"")
	filter "Debug Monolithic"
		defines("ZE_CONFIGURATION_NAME=\"Debug Monolithic\"")
	filter "Release"
		defines("ZE_CONFIGURATION_NAME=\"Release\"")
		defines("NDEBUG")
	filter "Release Editor"
		defines("ZE_CONFIGURATION_NAME=\"Release Editor\"")
		defines("NDEBUG")
	filter "Release Monolithic"
		defines("ZE_CONFIGURATION_NAME=\"Release Monolithic\"")
		defines("NDEBUG")
	filter {}
	
	language "C++"
	cppdialect "C++17"
	files { "**.h", "**.cpp", "**.hpp", "**.cxx", "**.inl" }
	vpaths { ["Sources/*"] = "**.h" }
	vpaths { ["Sources/*"] = "**.cpp" }
	vpaths { ["Sources/*"] = "**.inl" }
	vpaths { ["Sources/*"] = "**.hpp" }
	vpaths { ["Sources/*"] = "**.cxx" }
	
	targetdir (BinDir.."%{cfg.longname}")
	objdir (IntermediateDir.."%{prj.name}/".."%{cfg.longname}")
	debugdir(SrcDir.."/../")
	
	-- Default includes & libs
	includedirs "Public"
	includedirs "Private"
	libdirs(BinDir.."/%{cfg.longname}")
	
	-- Compiler
	exceptionhandling("Off")
	rtti("Off")
	
	-- Add required inc/libs
	mod:addIncludeDirs(RequiredIncludes)
	mod:addLibDirs(RequiredLibDirs)
	mod:addLibs(RequiredLibs)
	
	-- Public headers
	mod.publicHeaders = os.matchfiles("Public/**.h")
	
	-- Generate ZRT file + pre-create .gen.cpp to includes them into vs sln
	local ReflDir = BuildDir.."Reflection/"
	local modFile = io.open(ReflDir..mod.name..".zrt", "w+")
	
	configs = { "Debug", "Debug Monolithic", "Release", "Release Monolithic" }
	
	-- Parse all public headers and add them to the .zrt file
	for _, h in pairs(mod.publicHeaders) do
		local fullHeaderPath = mod.root.."/"..h
		local header = io.open(fullHeaderPath, "r")
		local content = header:read("*all")
		if string.find(content, "ZENUM") or string.find(content, "ZSTRUCT") or string.find(content, "ZCLASS") then
			modFile:write(fullHeaderPath.."\n")
			
			-- Precreate .gen.cpp if it doesn't exist so VS can detect it
			for _, c in pairs(configs) do
				local headerName = fullHeaderPath:match("^.+/(.+)$")
				local headerNameNoExt = headerName:match("^.+(%..+)$")
				local genCppPath = IntermediateDir..mod.name.."/"..c.."/Reflection/"..headerName:sub(1, -3)..".gen.cpp"
				if not os.isfile(genCppPath) then
					local genCpp = io.open(genCppPath, "w+")
					genCpp:write("/** DON'T MODIFY! */")
					genCpp:close()
				end
			end
		end
		header:close()
	end

	modFile:close()
	
	-- Reflection related
	includedirs { IntermediateDir..mod.name.."/%{cfg.longname}/Reflection/" }
	files { IntermediateDir..mod.name.."/%{cfg.longname}/Reflection/*.gen.cpp" }

	currentModule = mod
	
	table.insert(Modules, mod)
	
	return mod
end

function Module:addModules(deps)
	appendTables(self.deps, deps)
	
	for k, v in pairs(deps) do
		local path = searchModulePath(v)
		if path then
			includedirs(path.."/Public")
			includedirs { IntermediateDir..v.."/%{cfg.longname}/Reflection/" }
			dependson(v)
			filterModularOnly()
				defines(string.upper(v).."_API=__declspec(dllimport)")
				links("ZinoEngine-"..v)
			filterMonolithicOnly()
				defines(string.upper(v).."_API=")
			filterReset()
		else
			print("Can't find module "..v)
		end
	end
end

function Module:addIncludeDirs(includeDirs)
	appendTables(self.includeDirs, includeDirs)
	
	for k, v in pairs(includeDirs) do
		includedirs(v)
	end
end

function Module:addLibDirs(libDirs)
	appendTables(self.libDirs, libDirs)
	
	local oldFilter = currentFilter
	
	if currentFilter == 0 then
		filterDebugModularOnly()
		appendTables(self.debugLibDirs, libDirs)
	elseif currentFilter == 1 then
		appendTables(self.releaseLibDirs, libDirs)
		filterReleaseModularOnly()
	else
		filterModularOnly()
		appendTables(self.bothLibDirs, libDirs)
	end

	for k, v in pairs(libDirs) do
		libdirs(v)
	end
	
	filterApplyByIdx(oldFilter)
end

function Module:addLibs(libs)
	appendTables(self.libs, libs)
	
	local oldFilter = currentFilter
	
	if currentFilter == 0 then
		filterDebugModularOnly()
		appendTables(self.debugLibs, libs)
	elseif currentFilter == 1 then
		appendTables(self.releaseLibs, libs)
		filterReleaseModularOnly()
	else
		filterModularOnly()
		appendTables(self.bothLibs, libs)
	end
	
	for k, v in pairs(libs) do
		links(v)
	end

	filterApplyByIdx(oldFilter)
end

-- Search the specified module
-- Return the path if found or else nil
function searchModulePath(name)
	return os.pathsearch(name..".lua", SrcEnginePathStr)
end

local function finishModule()
	if currentModule.kind == "SharedLib" then
		-- Reflection command for this module
		zrtFileString = BuildDir.."Reflection/"..currentModule.name..".zrt "
		prebuildcommands { 
			"\""..BinDir.."%{cfg.longname}/ZinoEngine-ZinoReflectionTool.exe\" -Module="..currentModule.name.." -SrcDir=\""..currentModule.root.."\" -OutDir=\""..IntermediateDir..currentModule.name.."/%{cfg.longname}/Reflection/\" "..zrtFileString }
	end
end

local function executeModule(name)
	local path = searchModulePath(name)
	
	if path then
		include(path.."/"..name..".lua")
		finishModule()
		currentModule = nil
		filterReset()
	else
		print("Can't find module "..name)
	end
end

workspace "ZinoEngine"
	location ("../Build/ProjectFiles")
	configurations { "Debug", "Release", "Debug Monolithic", "Release Monolithic" }
	language "C++"
	architecture "x86_64"
	filter { "Debug or Release Editor or Debug Monolithic" }
		symbols "On"
	filter { "Release or Release Editor or Release Monolithic" }
		optimize "On"
	filter { }
	startproject "Main"
	
	targetprefix ("ZinoEngine-")
	group "Engine/Core"
		executeModule("EngineCore")
		executeModule("Engine")
		executeModule("Reflection")
	group "Engine/Rendering"
		executeModule("RenderCore")
		executeModule("RenderSystem")
		executeModule("VulkanRenderSystem")
		executeModule("Renderer")
	group "Engine/Rendering/Shaders"
		executeModule("Shaders")
		executeModule("ShaderCore")
		executeModule("ShaderCompiler")
		executeModule("VulkanShaderCompiler")
	group ""
		executeModule("Main")
	group "Tools"
		executeModule("ZinoReflectionTool")
	print("Total of "..#Modules.." module(s)")
	