#include "Engine/Assets/StaticMesh.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/Resources/Buffer.h"

namespace ZE
{

CStaticMeshRenderData::CStaticMeshRenderData(CStaticMesh* InStaticMesh) : StaticMesh(InStaticMesh) 
{
	const std::vector<SStaticMeshVertex>& Vertices = StaticMesh->Vertices;
	const std::vector<uint32_t>& Indices = StaticMesh->Indices;

	IndexFormat = EIndexFormat::Uint32;

	VertexBuffer = GRenderSystem->CreateBuffer({
		ERSBufferUsageFlagBits::VertexBuffer,
		ERSMemoryUsage::DeviceLocal,
		ERSMemoryHintFlagBits::None,
		Vertices.size() * sizeof(Vertices.front()) });
	if(!VertexBuffer)
		ZE::Logger::Error("Failed to create a vertex buffer");

	VertexBuffer->SetName("StaticMesh VertexBuffer");
	RSUtils::Copy(Vertices.data(), VertexBuffer.get());

	IndexBuffer = GRenderSystem->CreateBuffer({
		ERSBufferUsageFlagBits::IndexBuffer,
		ERSMemoryUsage::DeviceLocal,
		ERSMemoryHintFlagBits::None,
		Indices.size() * sizeof(Indices.front()) });
	if (!IndexBuffer)
		ZE::Logger::Error("Failed to create a index buffer");
	
	IndexBuffer->SetName("StaticMesh IndexBuffer");
	RSUtils::Copy(Indices.data(), IndexBuffer.get());
}

void CStaticMesh::UpdateData(const std::vector<SStaticMeshVertex>& InVertices,
	const std::vector<uint32_t>& InIndices)
{
	Vertices = InVertices;
	Indices = InIndices;
	IndexCount = static_cast<uint32_t>(Indices.size());

	RenderData = std::make_unique<CStaticMeshRenderData>(this);
}

} /* namespace ZE */