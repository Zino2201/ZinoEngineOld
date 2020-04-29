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

if not exist Sources/Libs/SPIRV-Tools git submodule init

echo Building 3rd party libs for VS2019... (requires CMake, Python 3 and Git)

REM MS Build
echo Locating VS...
setlocal enabledelayedexpansion
for /f "usebackq tokens=*" %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set MSBUILD=%%i
)

if not defined MSBUILD goto :msbuildnotfound

REM SPIRV-Tools
echo Building SPIRV-Tools
cd Sources/Libs/SPIRV-Tools/
echo Syncing deps
echo %ERRORLEVEL% > python3 utils/git-sync-deps 
echo Building using CMake
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64" /p:BuildInParallel=true
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64" /p:BuildInParallel=true

REM SPIRV-Cross
echo Building SPIRV-Cross
cd ../../SPIRV-Cross/
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64" /p:BuildInParallel=true
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64" /p:BuildInParallel=true

REM Glslang
echo Building glslang
cd ../../glslang/
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../ -DENABLE_CTEST=false -DBUILD_TESTING=false -DBUILD_EXTERNAL=false -DSKIP_GLSLANG_INSTALL=true
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64" /p:BuildInParallel=true
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64" /p:BuildInParallel=true

REM Shaderc
echo Building shaderc
cd ../../shaderc
echo Syncing deps 
echo %ERRORLEVEL% > python3 utils/git-sync-deps 
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../ -DENABLE_CTEST=false -DBUILD_GMOCK=false -DBUILD_TESTING=false -DSHADERC_SKIP_TESTS=true -DSHADERC_SPIRV_TOOLS_DIR=%ROOT%/Sources/Libs/SPIRV-Tools -DSHADERC_GLSLANG_DIR=%ROOT%/Sources/Libs/glslang -DSHADERC_ENABLE_SHARED_CRT=true
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64" /p:BuildInParallel=true
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64" /p:BuildInParallel=true
pause
exit

:msbuildnotfound
echo MS Build not found
pause
exit