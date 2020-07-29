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

class ENGINE_API CZEGameApp final : public CZinoEngineApp,
	public CTickable
{
public:
    CZEGameApp();
	~CZEGameApp();

	void Tick(const float& InDeltaTime) override;
    int OnWindowResized(SDL_Event* InEvent);
protected:
    void ProcessEvent(SDL_Event& InEvent) override;
	void Draw() override;
private:
	void OnVsyncChanged(const int32_t& InData);
private:
	std::unique_ptr<class CWorld> World;
	std::unique_ptr<CWindow> Window;
	std::unique_ptr<CViewport> Viewport;
	std::shared_ptr<class CStaticMesh> testSM;
	std::unique_ptr<ZE::UI::CImGuiRender> ImGuiRenderer;
	ImFont* Font;
};

} /* namespace ZE */