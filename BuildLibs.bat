@echo off

cd %~dp0
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

if not exist Sources/Libs/shaderc git submodule init

echo Building 3rd party libs for VS2019... (requires CMake, Python 3 and Git)

REM MS Build
echo Locating VS...
setlocal enabledelayedexpansion
for /f "usebackq tokens=*" %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set MSBUILD=%%i
)

if not defined MSBUILD goto :msbuildnotfound

REM Shader Conductor
echo Building Shader Conductor
cd Sources/Libs/ShaderConductor/
echo Debug
py BuildAll.py vs2019 vc150 x64 Debug
echo Release 
py BuildAll.py vs2019 vc150 x64 Release
pause
exit

echo Copying DLLs

:msbuildnotfound
echo MS Build not found
pause
exit