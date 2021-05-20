add_executable(zert
	Main.cpp
	Type.cpp
	Parser.cpp
	Writer.cpp)
target_include_directories(zert PRIVATE ${ZE_LIBS_DIR}/robin-hood-hashing/src/include)
target_compile_features(zert PRIVATE cxx_std_20)

# Enable SSE4.2
target_compile_options(zert PRIVATE -msse4.2)

set_target_properties(zert PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ZE_BINS_DIR})