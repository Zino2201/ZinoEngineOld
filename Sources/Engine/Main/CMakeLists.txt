find_package(SDL2 CONFIG REQUIRED)

ze_create_exec_module(ZinoEngine WIN32 Private/Main.cpp)

ze_add_private_modules(ZinoEngine 
	Core ZEFS ShaderCore ShaderCompiler VulkanShaderCompiler Engine GfxCore VulkanBackend)

target_link_libraries(ZinoEngine PRIVATE SDL2::SDL2)

if(ZE_WITH_EDITOR)
	ze_add_private_modules(ZinoEngine ZEEditor)
endif()

if(ZE_MONOLITHIC)
	target_link_libraries(ZinoEngine PRIVATE AllModules)
endif()
