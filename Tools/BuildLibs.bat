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

cd ../

echo Updating submodules
git submodule update --remote --merge

echo Building 3rd party libs using MSVC x64... (requires CMake, Python 3 and Git)

cd Sources/Libs/ShaderConductor/
echo Building Shader Conductor
echo Debug
py BuildAll.py vs2019 vc142 x64 Debug
echo Release 
py BuildAll.py vs2019 vc142 x64 Release
pause
exit