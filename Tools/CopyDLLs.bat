@echo off
echo Copying DLLs

cd ../

mkdir Binaries\Debug\ 2>NUL
mkdir Binaries\Release\ 2>NUL
mkdir Binaries\RelWithDebInfo\ 2>NUL
mkdir Binaries\RelWithDebInfo-Editor\ 2>NUL
mkdir Binaries\RelWithDebInfo-Monolithic\ 2>NUL
mkdir Binaries\Release-Monolithic\ 2>NUL

copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\ShaderConductor.dll Binaries\Debug-Editor\ShaderConductor.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\dxcompiler.dll Binaries\Debug-Editor\dxcompiler.dll

copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\Release\ShaderConductor.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\Release\dxcompiler.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\Release-Monolithic\ShaderConductor.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\Release-Monolithic\dxcompiler.dll

copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\RelWithDebInfo\ShaderConductor.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\RelWithDebInfo\dxcompiler.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\RelWithDebInfo-Editor\ShaderConductor.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\RelWithDebInfo-Monolithic\ShaderConductor.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\RelWithDebInfo-Editor\dxcompiler.dll
copy Sources\thirdparty\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\RelWithDebInfo-Monolithic\dxcompiler.dll

copy "Sources\thirdparty\nvidia-texture-tools\project\vc2017\Debug.x64\bin\nvtt.dll" Binaries\Debug-Editor\nvtt.dll
copy "Sources\thirdparty\nvidia-texture-tools\project\vc2017\Release.x64\bin\nvtt.dll" Binaries\RelWithDebInfo-Editor\nvtt.dll
copy "Sources\thirdparty\nvidia-texture-tools\project\vc2017\Release.x64\bin\nvtt.dll" Binaries\Release-Editor\nvtt.dll

pause