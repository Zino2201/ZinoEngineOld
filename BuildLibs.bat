@echo off

set ROOT=%CD%

color A

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
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64"
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64"

REM SPIRV-Cross
echo Building SPIRV-Cross
cd ../../SPIRV-Cross/
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64"
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64"

REM Glslang
echo Building glslang
cd ../../glslang/
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../ -DENABLE_CTEST=false -DBUILD_TESTING=false -DBUILD_EXTERNAL=false -DSKIP_GLSLANG_INSTALL=true
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64"
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64"

REM Shaderc
echo Building shaderc
cd ../../shaderc
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ../ -DENABLE_CTEST=false -DBUILD_GMOCK=false -DBUILD_TESTING=false -DSHADERC_SKIP_TESTS=true -DSHADERC_SPIRV_TOOLS_DIR=%ROOT%/Sources/Libs/SPIRV-Tools -DSHADERC_GLSLANG_DIR=%ROOT%/Sources/Libs/glslang -DSHADERC_ENABLE_SHARED_CRT=true
echo Debug
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Debug" /p:Platform="x64"
echo Release
"%MSBUILD%" ALL_BUILD.vcxproj /t:build /p:Configuration="Release" /p:Platform="x64"
pause
exit

:msbuildnotfound
echo MS Build not found
pause
exit