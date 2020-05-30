#pragma once

#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"
#include "Renderer/ProxyDrawCommand.h"

namespace ZE::Renderer
{

template<typename ShaderArray>
void CMeshRenderPass::BuildDrawCommand(CWorldProxy* InWorldProxy,
	CRenderableComponentProxy* InProxy,
	const CMeshCollection& InCollection,
	const size_t& InInstanceIdx,
	const ShaderArray& InShaderArray)
{
	const auto& Instance = InCollection.GetInstance(InInstanceIdx);

	CProxyDrawCommand DrawCommand(
		InCollection.GetPipeline(),
		InCollection.GetVertexBuffer(),
		InCollection.GetIndexBuffer(),
		Instance.IndexCount);

	DrawCommand.AddBinding(0, 1, InProxy->PerInstanceDataUBO.GetBuffer());

	CProxyDrawCommand* Command = InWorldProxy->GetDrawCommandManager().AddCommand(RenderPass,
		DrawCommand);

	InProxy->DrawCommands[RenderPass].insert(Command);
	InCollection.DrawCommands[RenderPass].insert(Command);
}

}