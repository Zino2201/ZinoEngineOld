@echo off
echo Generating solution and project files for Visual Studio 2019
cd Sources/
"../Tools/premake5.exe" vs2019
pause