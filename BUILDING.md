# Building ZinoEngine

ZinoEngine can be build on Windows and Linux using clang(-cl) and CMake.

## Pre-requisites
- Git
- CMake 3.10+ (for building the engine)
- Python 3+ (for building ShaderConductor/DXC)
- Vulkan SDK installed with `VULKAN_SDK` env var set

## Downloading
1. Clone the repository: `git clone https://github.com/Zino2201/ZinoEngine.git`
2. Download the submodules: `git submodule init`

## Building for Windows

### Pre-requisites
- Visual Studio 2019
- clang-cl

*vcpkg is also **strongly recommended** in order to manage some third-party libraries. If you don't use vcpkg, you will need to manually provide additional informations to CMake in order to include some third-party libraries*


### Steps

#### Third-party libraries installation

*If you use vcpkg, please read https://github.com/Microsoft/vcpkg#using-vcpkg-with-cmake to configure CMake*

- Install SDL2 (vcpkg: `vcpkg install sdl2[*]:x64-windows`)

#### Build

1. Execute `Tools/BuildLibs.bat` to update submodules and build some third-party libs

2. Execute cmake to generate project files (you can also directly use CLion or Visual Studio CMake integration). Example with a Debug Modular Editor build: `cmake -B Build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DZE_WITH_EDITOR=ON -DZE_MONOLITHIC=OFF`

3. Build the engine
4. Run it! (*current working directory must be the root directory, this may change later*)

## Building for Linux
TODO
## Misc
### CMake options
- ZE_WITH_EDITOR (ON | OFF) : Enable/disable editor modules
- ZE_MONOLITHIC (ON | OFF) : Enable/disable monolithic build