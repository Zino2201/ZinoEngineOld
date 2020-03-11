#pragma once

#include "Core/Asset.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "RenderCore.h"

class CRenderSystemBuffer;

/**
 * A static mesh vertex/index buffer
 */
class CStaticMeshVertexIndexBuffer final : public CRenderResource
{
public:
	void Init(const std::vector<SVertex>& InVertices,
		const std::vector<uint32_t>& InIndices);

	IRenderSystemVertexBuffer* GetVertexBuffer() const { return VertexBuffer.get(); }
	IRenderSystemIndexBuffer* GetIndexBuffer() const { return IndexBuffer.get(); }
	EIndexFormat GetOptimalIndexFormat() const { return IndexFormat; }
	uint32_t GetIndexCount() const { return static_cast<uint32_t>(Indices.size()); }
protected:
	virtual void InitRenderThread() override;
	virtual void DestroyRenderThread() override;
private:
	IRenderSystemVertexBufferPtr VertexBuffer;
	IRenderSystemIndexBufferPtr IndexBuffer;
	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	EIndexFormat IndexFormat;
};

class CStaticMesh : public IAsset
{
public:
	~CStaticMesh();

	virtual void Load(const std::string& InPath) override;

	const uint32_t& GetIndexCount() const { return IndexCount; }
	const std::vector<SVertex>& GetVertices() const { return Vertices; }
	const std::vector<uint32_t>& GetIndices() const { return Indices; }
	CStaticMeshVertexIndexBuffer* GetVertexIndexBuffer() const { return VertexIndexBuffer; }
private:
	std::vector<SVertex> Vertices;
	std::vector<uint32_t> Indices;
	CStaticMeshVertexIndexBuffer* VertexIndexBuffer;
	uint32_t IndexCount;
};