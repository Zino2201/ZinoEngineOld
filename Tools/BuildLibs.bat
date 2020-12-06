@echo off

cd /d "%~dp0"
set ROOT=%CD%

color A

REM CHECK ADMIN MODE
net session >NUL 2>&1
IF NOT %errorLevel% == 0 (
    COLOR 4
    ECHO WARNING: Not in administrator mode ! You may encounter problems during the build process.
    PAUSE
	color a
	cls
)

echo Locating VS...
setlocal enabledelayedexpansion
for /f "usebackq tokens=*" %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set MSBUILD=%%i
)

cd ../

echo Updating submodules
git submodule update --remote --merge

echo Building 3rd party libs using MSVC x64... (requires CMake, Python 3 and Git)

cd Sources/Libs/nvidia-texture-tools/project/vc2017
echo Building nvidia-texture-tools
"%MSBUILD%" nvtt.sln /p:Configuration="Debug" /p:Platform="x64" /p:PlatformToolset=v142
"%MSBUILD%" nvtt.sln /p:Configuration="Release" /p:Platform="x64" /p:PlatformToolset=v142
cd ../../../../../

cd Sources/Libs/ShaderConductor/
echo Building Shader Conductor
py BuildAll.py vs2019 vc142 x64 Debug
py BuildAll.py vs2019 vc142 x64 Release
pause
exit