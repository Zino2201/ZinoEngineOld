-- Useful globals
SrcDir = os.getcwd()
RootDir = path.join(SrcDir, "../")
LibDir = path.join(SrcDir, "Libs")
BuildDir = path.join(RootDir, "Build")
IntermediateDir = path.join(BuildDir, "Intermediates")
BinDir = path.join(RootDir, "Binaries")

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
		LibDir.."/glm",
		LibDir.."/stb_image",
		LibDir.."/SDL/include",
		LibDir.."/boost",
		LibDir.."/imgui",
		LibDir.."/robin-hood-hashing/src/include",
		LibDir.."/fmt/include",
	}
RequiredLibDirs = 
	{
		LibDir.."/SDL/build/build/.libs"
	}
RequiredLibs = 
	{
		"SDL2"
	}

currentFilter = -1

-- Utils functions
function filterDebugOnly()
	filter { "Debug or DebugMonolithic" }
	
	currentFilter = 0
end

function filterReleaseOnly()
	filter { "Release or ReleaseEditor or ReleaseMonolithic" }
	
	currentFilter = 1
end

function filterMonolithicOnly()
	filter { "DebugMonolithic or ReleaseMonolithic" }
	
	currentFilter = 2
end

function filterModularOnly()
	filter { "Debug or Release or ReleaseEditor" }
	
	currentFilter = 3
end

function filterDebugModularOnly()
	filter { "Debug or DebugEditor" }
	
	currentFilter = 4
end

function filterReleaseModularOnly()
	filter { "Release or ReleaseEditor" }
	
	currentFilter = 5
end

function filterDebugMonolithicOnly()
	filter { "DebugMonolithic" }
	
	currentFilter = 6
end

function filterReleaseMonolithicOnly()
	filter { "ReleaseMonolithic" }
	
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
	-- Skip module if it is none and we are on Linux
	if os.istarget("linux") and modKind == "None" then
		return
	end

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
		defines(string.upper(mod.name).."_API=ZE_DLLEXPORT")
	filterReset()
	
	-- Add a define for each configurations
	filter "Debug"
		defines("ZE_CONFIGURATION_NAME=\"Debug\"")
		defines("ZE_DEBUG")
	filter "DebugEditor"
		defines("ZE_CONFIGURATION_NAME=\"Debug Editor\"")
		defines("ZE_DEBUG")
		defines("ZE_EDITOR")
	filter "DebugMonolithic"
		defines("ZE_CONFIGURATION_NAME=\"Debug Monolithic\"")
		defines("ZE_DEBUG")
	filter "Release"
		defines("ZE_CONFIGURATION_NAME=\"Release\"")
		defines("NDEBUG")
		defines("ZE_RELEASE")
	filter "ReleaseEditor"
		defines("ZE_CONFIGURATION_NAME=\"Release Editor\"")
		defines("NDEBUG")
		defines("ZE_RELEASE")
		defines("ZE_EDITOR")
	filter "ReleaseMonolithic"
		defines("ZE_CONFIGURATION_NAME=\"Release Monolithic\"")
		defines("NDEBUG")
		defines("ZE_RELEASE")
	filter {}
	
	language "C++"
	
	-- C++20 not properly supported yet by premake5
	filter { "toolset:msc"}
		cppdialect "c++latest"
	filter { "action:gmake*" }
		buildoptions { "-std=c++20" }
	filter {}

	files { "**.h", "**.cpp", "**.hpp", "**.cxx", "**.inl" }
	vpaths { ["Sources/*"] = "**.h" }
	vpaths { ["Sources/*"] = "**.cpp" }
	vpaths { ["Sources/*"] = "**.inl" }
	vpaths { ["Sources/*"] = "**.hpp" }
	vpaths { ["Sources/*"] = "**.cxx" }
	
	targetdir (BinDir.."/%{cfg.longname}")
	objdir (IntermediateDir.."/%{prj.name}/".."%{cfg.longname}")
	debugdir(RootDir)
	
	-- Default includes & libs
	includedirs "Public"
	includedirs "Private"
	libdirs(BinDir.."/%{cfg.longname}")
	filter { "action:gmake* "}
		linkoptions("-Wl,-rpath,./ -Wl,-rpath,Binaries/%{cfg.longname}")
	filter {}
	
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
	local ReflDir = BuildDir.."/Reflection/"
	local modFile = io.open(ReflDir..mod.name..".zrt", "w+")
	
	configs = { "Debug", "DebugMonolithic", "Release", "ReleaseMonolithic" }
	
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
				local genCppPath = IntermediateDir.."/"..mod.name.."/"..c.."/Reflection/"..headerName:sub(1, -3)..".gen.cpp"
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
	includedirs { IntermediateDir.."/"..mod.name.."/%{cfg.longname}/Reflection/" }
	files { IntermediateDir.."/"..mod.name.."/%{cfg.longname}/Reflection/*.gen.cpp" }

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
			includedirs { IntermediateDir.."/"..v.."/%{cfg.longname}/Reflection/" }
			dependson(v)
			filterModularOnly()
				defines(string.upper(v).."_API=ZE_DLLIMPORT")
				links(v)
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
		filter { "action:gmake*" }
		linkoptions("-Wl,-rpath-link,"..v)
		libdirs(v)
		filter {}
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
	if not currentModule then return end

	if currentModule.kind == "SharedLib" then
		-- Reflection command for this module
		zrtFileString = BuildDir.."/Reflection/"..currentModule.name..".zrt "
		filter { "toolset: msc" }
			prebuildcommands { 
				"\""..BinDir.."/%{cfg.longname}/ZinoReflectionTool.exe\" -Module="..currentModule.name.." -SrcDir=\""..currentModule.root.."\" -OutDir=\""..IntermediateDir.."/"..currentModule.name.."/%{cfg.longname}/Reflection/\" "..zrtFileString }
		filter { "action:gmake*"}
		prebuildcommands { 
			"\""..BinDir.."/%{cfg.longname}/ZinoReflectionTool\" -Module="..currentModule.name.." -SrcDir=\""..currentModule.root.."\" -OutDir=\""..IntermediateDir.."/"..currentModule.name.."/%{cfg.longname}/Reflection/\" "..zrtFileString }
		filter {}
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
	configurations { "Debug", "Release", "DebugMonolithic", "ReleaseMonolithic" }
	language "C++"
	architecture "x86_64"
	filter { "Debug or ReleaseEditor or DebugMonolithic" }
		symbols "On"
	filter { "Release or ReleaseEditor or ReleaseMonolithic" }
		optimize "On"
	filter { }
	startproject "Main"

	-- Use clang by default on gmake compatible platforms
	filter { "action:gmake*"}
		toolset "clang"
	filter {}

	group "Engine/Core"
		executeModule("EngineCore")
		executeModule("Engine")
		executeModule("Reflection")
	group "Engine/Rendering"
		executeModule("RenderCore")
		executeModule("RenderSystem")
		executeModule("VulkanRenderSystem")
		executeModule("Renderer")
	group "Engine/Rendering/UI"
		executeModule("ImGui")
	group "Engine/Rendering/Shaders"
		executeModule("Shaders")
		executeModule("ShaderCore")
		executeModule("ShaderCompiler")
		executeModule("VulkanShaderCompiler")
	group "Editor"
		executeModule("Editor")
		executeModule("MapEditor")
	group ""
		executeModule("Main")
	group "Tools"
		executeModule("ZinoReflectionTool")
	print("Total of "..#Modules.." module(s)")
	