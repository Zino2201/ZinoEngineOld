#include "Renderer/MeshCollection.h"

namespace ZE::Renderer 
{

CMeshCollection::CMeshCollection(CRSGraphicsPipeline* InPipeline,
	CRSBuffer* InVertexBuffer,
	CRSBuffer* InIndexBuffer,
	EIndexFormat InIndexFormat) : Pipeline(InPipeline), VertexBuffer(InVertexBuffer),
		IndexBuffer(InIndexBuffer), IndexFormat(InIndexFormat) {}

size_t CMeshCollection::AddInstance(const TOwnerPtr<CRenderableComponentProxy>& InProxy,
	const uint32_t& InIndexCount, const uint32_t& InVertexCount, EMeshRenderPass InRenderPassFlags)
{
	Instances.emplace_back(InProxy, InIndexCount, InVertexCount,
		InRenderPassFlags);

	return Instances.size() - 1;
}

}