add_library(asset
    public/assets/AssetArchive.h
    public/assets/Asset.h
    public/assets/AssetManager.h
    public/assets/AssetPtr.h
    public/assets/AssetMetadata.h
    private/assets/Asset.cpp
    private/assets/AssetCooker.cpp
    private/assets/AssetManager.cpp)

target_link_libraries(asset PUBLIC core reflection zefs json platform)

target_include_directories(asset
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/stduuid/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)