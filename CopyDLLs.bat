@echo off
echo Copying DLLs

mkdir Binaries\Debug\ 2>NUL
mkdir Binaries\Release\ 2>NUL
mkdir Binaries\DebugMonolithic\ 2>NUL
mkdir Binaries\ReleaseMonolithic\ 2>NUL

copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\ShaderConductor.dll Binaries\Debug\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\dxcompiler.dll Binaries\Debug\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\ShaderConductor.dll Binaries\DebugMonolithic\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\dxcompiler.dll Binaries\DebugMonolithic\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\Release\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\Release\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\ReleaseMonolithic\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\ReleaseMonolithic\dxcompiler.dll

copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\Debug\SDL2.dll
copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\DebugMonolithic\SDL2.dll
copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\Release\SDL2.dll
copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\ReleaseMonolithic\SDL2.dll

pause