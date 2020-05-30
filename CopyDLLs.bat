@echo off
echo Copying DLLs

copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Debug\ShaderConductor.dll Binaries\Debug\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Debug\dxcompiler.dll Binaries\Debug\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Debug\ShaderConductor.dll Binaries\Debug Monolithic\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Debug\dxcompiler.dll Binaries\Debug Monolithic\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Release\ShaderConductor.dll Binaries\Release\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Release\dxcompiler.dll Binaries\Release\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Release Monolithic\ShaderConductor.dll Binaries\Release Monolithic\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc150-x64\Bin\Release Monolithic\dxcompiler.dll Binaries\Release Monolithic\dxcompiler.dll

copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\Debug\SDL2.dll
copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\Debug Monolithic\SDL2.dll
copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\Release\SDL2.dll
copy Sources\Libs\SDL2\lib\SDL2.dll Binaries\Release Monolithic\SDL2.dll

copy Sources\Libs\assimp\lib\assimp-vc142-mt.dll Binaries\Debug\assimp-vc142-mt.dll
copy Sources\Libs\assimp\lib\assimp-vc142-mt.dll Binaries\Debug Monolithic\assimp-vc142-mt.dll
copy Sources\Libs\assimp\lib\assimp-vc142-mt.dll Binaries\Release\assimp-vc142-mt.dll
copy Sources\Libs\assimp\lib\assimp-vc142-mt.dll Binaries\Release Monolithic\assimp-vc142-mt.dll
pause