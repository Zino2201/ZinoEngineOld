#include "Engine/StaticMesh.h"
#include "Render/RenderSystem/RenderSystem.h"

namespace ZE
{

DECLARE_LOG_CATEGORY(StaticMesh);

void CStaticMeshRenderData::InitResource_RenderThread()
{
	const std::vector<SStaticMeshVertex>& Vertices = StaticMesh->Vertices;
	const std::vector<uint32_t>& Indices = StaticMesh->Indices;

	IndexFormat = Indices.size() > std::numeric_limits<uint16_t>::max()
		? EIndexFormat::Uint32 : EIndexFormat::Uint16;

	VertexBuffer = GRenderSystem->CreateBuffer(
		ERSBufferUsage::VertexBuffer,
		ERSMemoryUsage::DeviceLocal,
		Vertices.size() * sizeof(Vertices.front()),
		SRSResourceCreateInfo(Vertices.data(), "StaticMeshVertexBuffer"));
	if(!VertexBuffer)
		LOG(ELogSeverity::Fatal, StaticMesh, "Failed to create a vertex buffer");

	IndexBuffer = GRenderSystem->CreateBuffer(
		ERSBufferUsage::IndexBuffer,
		ERSMemoryUsage::DeviceLocal,
		Indices.size() * sizeof(Indices.front()),
		SRSResourceCreateInfo(Indices.data(), "StaticMeshIndexBuffer"));
	if (!IndexBuffer)
		LOG(ELogSeverity::Fatal, StaticMesh, "Failed to create a index buffer");
}

void CStaticMeshRenderData::DestroyResource_RenderThread()
{
	VertexBuffer.reset();
	IndexBuffer.reset();
}

void CStaticMesh::UpdateData(const std::vector<SStaticMeshVertex>& InVertices,
	const std::vector<uint32_t>& InIndices)
{
	Vertices = InVertices;
	Indices = InIndices;
	IndexCount = static_cast<uint32_t>(Indices.size());

	RenderData = std::make_unique<CStaticMeshRenderData>(this);
	RenderData->InitResource();
}

} /* namespace ZE */