#include "RenderCommands.h"

void CRenderCommandBeginRecording::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->Begin();
}

void CRenderCommandEndRecording::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->End();
}

void CRenderCommandBeginRenderPass::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->BeginRenderPass(ClearColor);
}

void CRenderCommandEndRenderPass::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->EndRenderPass();
}

void CRenderCommandBindGraphicsPipeline::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->BindGraphicsPipeline(GraphicsPipeline.get());
}

void CRenderCommandDraw::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->Draw(VertexCount,
		InstanceCount, FirstVertex, FirstInstance);
}