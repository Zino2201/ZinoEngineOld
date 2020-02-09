#include "RenderCommands.h"
#include "Render/ShaderAttributesManager.h"

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
	InCmdList->GetCommandContext()->BindGraphicsPipeline(GraphicsPipeline);
}

void CRenderCommandBindVertexBuffers::Execute(CRenderCommandList* InCmdList) 
{
	InCmdList->GetCommandContext()->BindVertexBuffers(VertexBuffers);
}

void CRenderCommandBindIndexBuffer::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->BindIndexBuffer(IndexBuffer,
		Offset, IndexFormat);
}

void CRenderCommandDraw::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->Draw(VertexCount,
		InstanceCount, FirstVertex, FirstInstance);
}

void CRenderCommandDrawIndexed::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->DrawIndexed(IndexCount,
		InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void CRenderCommandSetShaderAttributeResource::Execute(CRenderCommandList* InCmdList)
{
	ShaderAttributesManager->Set(Stage, Name, Resource);
}

void CRenderCommandUpdateUniformBuffer::Execute(CRenderCommandList* InCmdList)
{
	if(UniformBuffer->GetInfos().bUsePersistentMapping)
		memcpy(UniformBuffer->GetMappedMemory(), Data, DataSize);
	else
	{
		void* Dst = UniformBuffer->Map();
		memcpy(Dst, Data, DataSize);
		UniformBuffer->Unmap();
	}
}

void CRenderCommandBindShaderAttributeManager::Execute(CRenderCommandList* InCmdList)
{
	InCmdList->GetCommandContext()->BindShaderAttributesManager(ShaderAttributesManager);
}