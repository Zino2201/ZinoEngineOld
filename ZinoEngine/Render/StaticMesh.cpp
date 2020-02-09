#include "StaticMesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "RenderCore.h"
#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"

void CStaticMesh::Load(const std::string& InPath)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(InPath, aiProcess_Triangulate | aiProcess_FlipUVs);

	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::unordered_map<SVertex, uint32_t> UniqueVertices;

	aiMesh* Mesh = Scene->mMeshes[0];

	Vertices.reserve(Mesh->mNumVertices);

	/** Vertices */
	for (uint32_t i = 0; i < Mesh->mNumVertices; ++i)
	{
		aiVector3D& Position = Mesh->mVertices[i];
		aiVector3D& Normal = Mesh->mNormals[i];
		aiVector3D& TexCoord = Mesh->mTextureCoords[0][i];
		
		SVertex Vertex;

		Vertex.Position = glm::vec3(Position.x, Position.y, Position.z);
		Vertex.TexCoord = glm::vec2(TexCoord.x, TexCoord.y);
		Vertex.Normal = glm::vec3(Normal.x, Normal.y, Normal.z);

		if (UniqueVertices.count(Vertex) == 0) 
		{
			UniqueVertices[Vertex] = static_cast<uint32_t>(Vertices.size());
			Vertices.push_back(Vertex);
		}

		Indices.emplace_back(UniqueVertices[Vertex]);
	}

	/** Create vertex buffer using staging buffer */
	{
		CRenderSystemBufferPtr StagingBuffer = CEngine::Get().GetRenderSystem()->CreateBuffer(
			SRenderSystemBufferInfos(
				sizeof(Vertices[0]) * Vertices.size(),
				EBufferUsage::TransferSrc,
				EBufferMemoryUsage::CpuToGpu));

		void* Data = StagingBuffer->Map();
		memcpy(Data, Vertices.data(), sizeof(Vertices[0]) * Vertices.size());
		StagingBuffer->Unmap();

		VertexBuffer = CEngine::Get().GetRenderSystem()->CreateBuffer(
			SRenderSystemBufferInfos(
				sizeof(Vertices[0]) * Vertices.size(),
				EBufferUsage::VertexBuffer | EBufferUsage::TransferDst,
				EBufferMemoryUsage::GpuOnly));

		VertexBuffer->Copy(StagingBuffer.get());
	}

	/** Create index buffer using staging buffer */
	{
		CRenderSystemBufferPtr StagingBuffer = CEngine::Get().GetRenderSystem()->CreateBuffer(
			SRenderSystemBufferInfos(
				sizeof(Indices[0]) * Indices.size(),
				EBufferUsage::TransferSrc,
				EBufferMemoryUsage::CpuToGpu));

		void* Data = StagingBuffer->Map();
		memcpy(Data, Indices.data(), sizeof(Indices[0]) * Indices.size());
		StagingBuffer->Unmap();

		IndexBuffer = CEngine::Get().GetRenderSystem()->CreateBuffer(
			SRenderSystemBufferInfos(
				sizeof(Indices[0]) * Indices.size(),
				EBufferUsage::IndexBuffer | EBufferUsage::TransferDst,
				EBufferMemoryUsage::GpuOnly));

		IndexBuffer->Copy(StagingBuffer.get());
	}

	IndexCount = static_cast<uint32_t>(Indices.size());
	IndexFormat = EIndexFormat::Uint16;

	if(IndexCount > std::numeric_limits<uint16_t>::max())
		IndexFormat = EIndexFormat::Uint32;
}