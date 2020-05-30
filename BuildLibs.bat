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

echo Updating submodules
git submodule update --init --recursive

echo Building 3rd party libs for VS2019 x64... (requires CMake, Python 3 and Git)

REM Shader Conductor
echo Building Shader Conductor
echo Debug
py Sources/Libs/ShaderConductor/BuildAll.py vs2019 vc150 x64 Debug
echo Release 
py Sources/Libs/ShaderConductor/BuildAll.py vs2019 vc150 x64 Release
pause
exit