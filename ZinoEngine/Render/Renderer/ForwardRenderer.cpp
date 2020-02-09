#include "ForwardRenderer.h"
#include "Render/Commands/RenderCommands.h"
#include "Core/Engine.h"
#include "Core/RenderThread.h"

std::array<float, 4> ClearColor = { 0.f, 0.f, 0.f, 1.0f };

void CForwardRenderer::Render(CRenderCommandList* InCommandList)
{
	/** Begin BassPass */
	InCommandList->Enqueue<CRenderCommandBeginRenderPass>(ClearColor);
	DrawWorld(InCommandList);
	InCommandList->Enqueue<CRenderCommandEndRenderPass>();
}

void CForwardRenderer::DrawWorld(CRenderCommandList* InCommandList)
{
	for(CRenderableComponentProxy* Proxy : 
		CEngine::Get().GetSceneProxy()->GetRenderableComponents())
	{
		Proxy->Draw(InCommandList);
	}
}