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

	/** 
	 * Generate draw command 
	 * Allocate to the scene global mesh draw commands and also add a pointer to the proxy
	 * for deleting the command when required 
	 */
	CProxyDrawCommand& DrawCommand = InWorldProxy->GetDrawCommandManager().AddCommand(RenderPass,
			InCollection.GetPipeline(),
			InCollection.GetVertexBuffer(),
			InCollection.GetIndexBuffer(),
			InCollection.GetIndexFormat(),
			Instance.IndexCount);

	DrawCommand.AddBinding(0, 1, InProxy->PerInstanceDataUBO.GetBuffer());

	InProxy->DrawCommands[RenderPass].insert(&DrawCommand);
}

}