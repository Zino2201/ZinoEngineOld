add_executable(ZERT Private/Main.cpp
	Private/Type.cpp
	Private/Parser.cpp
	Private/Writer.cpp)
target_include_directories(ZERT 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private
	        ${ZE_LIBS_DIR}/robin-hood-hashing/src/include)
target_compile_features(ZERT PRIVATE cxx_std_20)

# Enable SSE4.2
target_compile_options(ZERT PRIVATE -msse4.2)

set_target_properties(ZERT
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ZE_BINS_DIR})