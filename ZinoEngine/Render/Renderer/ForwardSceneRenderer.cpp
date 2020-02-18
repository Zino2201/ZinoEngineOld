#include "ForwardSceneRenderer.h"
#include "Render/Commands/RenderCommandContext.h"
#include "Core/Engine.h"
#include "Core/RenderThread.h"
#include "Render/Renderer/Scene.h"
#include "Render/World/RenderableComponentProxy.h"

std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

void CForwardSceneRenderer::Render(IRenderCommandContext* InCommandContext,
	CScene* InScene)
{
	/** Begin BassPass */
	InCommandContext->BeginRenderPass(ClearColor, "BasePass");
	DrawWorld(InCommandContext, InScene);
	InCommandContext->EndRenderPass();
}

void CForwardSceneRenderer::DrawWorld(IRenderCommandContext* InCommandContext,
	CScene* InScene)
{
	for(const auto& [Component, Proxy] : InScene->GetRenderableComponents())
	{
		Proxy->Draw(InCommandContext);
	}
}