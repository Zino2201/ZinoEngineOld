@echo off
echo Copying DLLs

cd ../

mkdir Binaries\Debug\ 2>NUL
mkdir Binaries\Release\ 2>NUL
mkdir Binaries\RelWithDebInfo\ 2>NUL
mkdir Binaries\RelWithDebInfo-Editor\ 2>NUL
mkdir Binaries\ReleaseMonolithic\ 2>NUL

copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\ShaderConductor.dll Binaries\Debug\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Debug\dxcompiler.dll Binaries\Debug\dxcompiler.dll

copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\Release\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\Release\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\ReleaseMonolithic\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\ReleaseMonolithic\dxcompiler.dll

copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\RelWithDebInfo\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\RelWithDebInfo\dxcompiler.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\ShaderConductor.dll Binaries\RelWithDebInfo-Editor\ShaderConductor.dll
copy Sources\Libs\ShaderConductor\Build\vs2019-win-vc142-x64\Bin\Release\dxcompiler.dll Binaries\RelWithDebInfo-Editor\dxcompiler.dll

pause