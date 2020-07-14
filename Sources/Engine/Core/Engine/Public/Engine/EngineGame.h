#pragma once

#include "Engine.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include <future>
#include "Shader/ShaderCompiler.h"
#include "Render/UniformBuffer.h"
#include "Renderer/WorldRenderer.h"
#include "ImGui/ImGuiRender.h"
#include "Console/Console.h"

struct ImFont;

namespace ZE
{

class CWindow;
class CViewport;

/**
 * Engine derived class for games
 */
class CEngineGame : public CEngine
{
public:
    ~CEngineGame();

    ENGINE_API virtual void Initialize() override;
    ENGINE_API virtual void Tick(const float& InDeltaTime) override;
    ENGINE_API virtual void Draw() override;
    ENGINE_API void ProcessEvent(SDL_Event* InEvent) override;
    ENGINE_API void Exit() override;
    int OnWindowResized(SDL_Event* InEvent);
private:
    void OnVsyncChanged(const int32_t& InData);
private:
    std::unique_ptr<class CWorld> World;
    CWindow* Window;
    std::unique_ptr<CViewport> Viewport;
    SRSRenderPass TestRenderPass;
	bool bshouldrendertri = false;
	CRSShaderPtr shaderV = nullptr;
	CRSShaderPtr shaderF = nullptr;
	CRSGraphicsPipelinePtr pipeline = nullptr;
    std::future<SShaderCompilerOutput> Vertex;
    std::future<SShaderCompilerOutput> Frag;
    CRSBufferPtr ubo = nullptr;
    std::shared_ptr<class CStaticMesh> testSM;
    std::unique_ptr<ZE::UI::CImGuiRender> ImGuiRenderer;
    ImFont* Font;
    std::mutex ProfilingMutex;
};

} /* namespace ZE */