@echo off
echo Generating solution and project files for Visual Studio 2019
cd Sources/
"../premake5.exe" vs2019
pause