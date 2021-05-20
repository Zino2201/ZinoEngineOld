add_library(assetdatacache
        private/assetdatacache/AssetDatacache.cpp
        public/assetdatacache/AssetDatacache.h)
target_link_libraries(assetdatacache PRIVATE core zefs)
target_include_directories(assetdatacache
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)