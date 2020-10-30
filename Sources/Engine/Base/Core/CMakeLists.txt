find_package(glm REQUIRED PATHS ${ZE_LIBS_DIR}/glm/cmake/glm)

ze_create_module(Core 
    Private/EngineCore.cpp Private/App.cpp Private/StringUtil.cpp
    Private/Console/Console.cpp
    Private/Logger/Sinks/WinDbgSink.cpp
    Private/Logger/Logger.cpp Private/Logger/Sink.cpp
    Private/Memory/SmartPointers.cpp
    Private/Module/ModuleManager.cpp
    Private/Serialization/BinaryArchive.cpp
    Private/Threading/JobSystem/Job.cpp Private/Threading/JobSystem/JobSystem.cpp Private/Threading/JobSystem/WorkerThread.cpp
    Private/Threading/Thread.cpp Private/MessageBox.cpp)

target_include_directories(Core
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/fmt/include
        ${ZE_LIBS_DIR}/boost
        ${ZE_LIBS_DIR}/intrusive
        ${ZE_LIBS_DIR}/robin-hood-hashing/src/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)
target_compile_features(Core PUBLIC cxx_std_20)
target_include_directories(Core PUBLIC ${GLM_INCLUDE_DIRS})
target_compile_definitions(Core PUBLIC "-DFMT_EXCEPTIONS=0")