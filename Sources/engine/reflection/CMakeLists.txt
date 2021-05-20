add_library(reflection
    Private/Reflection/Registration.cpp
    Private/Reflection/Type.cpp
    Private/Reflection/Reflection.cpp
    Private/Reflection/Class.cpp
    Private/Reflection/Enum.cpp
    Private/Reflection/Property.cpp
    public/reflection/detail/AnyImpl.h
    public/reflection/detail/PropertyImpl.h
    public/reflection/Any.h
    public/reflection/BinaryArchiveRefl.h
    public/reflection/Builders.h
    public/reflection/Cast.h
    public/reflection/Enum.h
    public/reflection/Macros.h
    public/reflection/Property.h
    public/reflection/Registration.h
    public/reflection/Serialization.h
    public/reflection/Singleton.h
    public/reflection/Traits.h
    public/reflection/Type.h
    public/reflection/VectorRefl.h)

target_link_libraries(reflection PUBLIC core)

target_include_directories(reflection
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)