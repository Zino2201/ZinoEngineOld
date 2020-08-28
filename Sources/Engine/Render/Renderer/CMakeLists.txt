ze_create_module(Renderer Private/Renderer/RenderableComponentProxy.cpp
    Private/Renderer/RenderableComponentType.cpp Private/Renderer/RendererModule.cpp 
        Private/Renderer/WorldProxy.cpp Private/Renderer/WorldRenderer.cpp
    Private/Renderer/ClusteredForward/BasePass.cpp Private/Renderer/ClusteredForward/BasePassDrawcallFactory.cpp 
        Private/Renderer/ClusteredForward/ClusteredForwardRenderingPath.cpp
    Private/Renderer/FrameGraph/FrameGraph.cpp Private/Renderer/FrameGraph/RenderPass.cpp
    Private/Renderer/MeshRendering/RenderPassDrawcallFactory.cpp
    Private/Renderer/PostProcess/BarrelDistortion.cpp
    Private/Renderer/RenderPass/RenderPassRenderer.cpp)
ze_add_public_modules(Renderer Core RenderCore RenderSystem ShaderCore ShaderCompiler ImGui)

target_include_directories(Renderer
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)