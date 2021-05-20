add_library(assetdatabase
    public/assetdatabase/AssetDatabase.h
    private/assetdatabase/AssetDatabase.cpp
    private/assetdatabase/PathTree.cpp)

target_link_libraries(assetdatabase
    PUBLIC
        core
        reflection
        asset
        json
    PRIVATE
        zefs)

target_include_directories(assetdatabase
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)