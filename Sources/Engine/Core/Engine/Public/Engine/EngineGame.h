#pragma once

#include "Engine.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include <future>
#include "Shader/ShaderCompiler.h"

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
    ENGINE_API virtual void Tick(SDL_Event* InEvent, const float& InDeltaTime) override;
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
    std::unique_ptr<class CStaticMesh> testSM;
};

} /* namespace ZE */