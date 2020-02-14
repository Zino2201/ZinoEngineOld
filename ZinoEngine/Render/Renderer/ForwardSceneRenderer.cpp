#include "ForwardSceneRenderer.h"
#include "Render/Commands/RenderCommands.h"
#include "Core/Engine.h"
#include "Core/RenderThread.h"
#include "Render/Renderer/Scene.h"

std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

void CForwardSceneRenderer::Render(CRenderCommandList* InCommandList,
	CScene* InScene)
{
	/** Begin BassPass */
	InCommandList->Enqueue<CRenderCommandBeginRenderPass>(ClearColor);
	DrawWorld(InCommandList, InScene);
	InCommandList->Enqueue<CRenderCommandEndRenderPass>();
}

void CForwardSceneRenderer::DrawWorld(CRenderCommandList* InCommandList,
	CScene* InScene)
{
	for(const auto& [Component, Proxy] : InScene->GetRenderableComponents())
	{
		Proxy->Draw(InCommandList);
	}
}