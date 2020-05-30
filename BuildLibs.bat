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

if not exist Sources/Libs/ShaderConductor git submodule init

echo Building 3rd party libs for VS2019 x64... (requires CMake, Python 3 and Git)

REM Shader Conductor
echo Building Shader Conductor
cd Sources/Libs/ShaderConductor/
echo Debug
py BuildAll.py vs2019 vc150 x64 Debug
echo Release 
py BuildAll.py vs2019 vc150 x64 Release
pause
exit