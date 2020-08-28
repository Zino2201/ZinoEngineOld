add_executable(ZinoReflectionTool 
	Private/Header.cpp Private/main.cpp Private/Parser.cpp Private/Struct.cpp Private/Writer.cpp)
target_include_directories(ZinoReflectionTool 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private
	        ${ZE_LIBS_DIR}/robin-hood-hashing/src/include)
target_compile_features(ZinoReflectionTool PRIVATE cxx_std_20)

# Enable SSE4.2
target_compile_options(ZinoReflectionTool PRIVATE -msse4.2)

set_target_properties(ZinoReflectionTool
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ZE_BINS_DIR})