add_library(core
    private/EngineCore.cpp
    private/App.cpp
    private/StringUtil.cpp
    private/console/Console.cpp
    private/logger/Sinks/StdSink.cpp
    private/logger/Sinks/WinDbgSink.cpp
    private/logger/Logger.cpp
    private/logger/Sink.cpp
    private/memory/SmartPointers.cpp
    private/module/ModuleManager.cpp
    private/serialization/BinaryArchive.cpp
    private/threading/jobsystem/Job.cpp
    private/threading/jobsystem/JobSystem.cpp
    private/threading/jobsystem/WorkerThread.cpp
    private/threading/Thread.cpp
    private/MessageBox.cpp
    public/maths/matrix/Transformations.h
    public/maths/Color.h
    public/serialization/types/Uuid.h)

target_include_directories(core
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/fmt/include
        ${ZE_LIBS_DIR}/boost
        ${ZE_LIBS_DIR}/intrusive
        ${ZE_LIBS_DIR}/robin-hood-hashing/src/include
        ${ZE_LIBS_DIR}/stduuid/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
)
target_compile_features(core PUBLIC cxx_std_20)
target_compile_definitions(core PUBLIC "-DFMT_EXCEPTIONS=0")