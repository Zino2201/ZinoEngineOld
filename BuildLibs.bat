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

echo Building SDL2...
cd Sources/Libs/SDL
mkdir build
cd build
cmake ../

REM Shader Conductor
cd Sources/Libs/ShaderConductor/
echo Building Shader Conductor
echo Debug
py BuildAll.py vs2019 vc142 x64 Debug
echo Release 
py BuildAll.py vs2019 vc142 x64 Release
pause
exit