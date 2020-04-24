#pragma once

#include "EngineCore.h"
#include "Render/RenderThreadResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE
{

class CStaticMesh;

/**
 * A static mesh's vertex
 */
struct SStaticMeshVertex
{
    Math::SVector3Float Position;
    Math::SVector3Float Color;

    bool operator==(const SStaticMeshVertex& InOther) const
    {
        return Position == InOther.Position &&
            Color == InOther.Color;
    }

	static std::vector<SVertexInputBindingDescription> GetBindingDescriptions()
	{
		return
		{
			SVertexInputBindingDescription(0, sizeof(SStaticMeshVertex), 
                EVertexInputRate::Vertex),
		};
	}

	static std::vector<SVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		return
		{
			SVertexInputAttributeDescription(0, 0, EFormat::R32G32B32Sfloat,
				offsetof(SStaticMeshVertex, Position)),
			SVertexInputAttributeDescription(0, 1, EFormat::R32G32B32Sfloat,
			    offsetof(SStaticMeshVertex, Color)),
		};
	}
};

struct SStaticMeshVertexHash
{
	std::size_t operator()(const SStaticMeshVertex& InVertex) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine<Math::SVector3Float, Math::SVector3FloatHash>(Seed, InVertex.Position);
		HashCombine<Math::SVector3Float, Math::SVector3FloatHash>(Seed, InVertex.Color);

		return Seed;
	}
};

/**
 * Render data of a static mesh
 */
class CStaticMeshRenderData : public CRenderThreadResource
{
public:
    CStaticMeshRenderData(CStaticMesh* InStaticMesh) : StaticMesh(InStaticMesh) {}

    virtual void InitResource_RenderThread() override;
    virtual void DestroyResource_RenderThread() override;

    CRSBuffer* GetVertexBuffer() const { return VertexBuffer.get(); }
    CRSBuffer* GetIndexBuffer() const { return IndexBuffer.get(); }
    const EIndexFormat& GetIndexFormat() const { return IndexFormat; }
private:
    CStaticMesh* StaticMesh;
    CRSBufferPtr VertexBuffer;
    CRSBufferPtr IndexBuffer;
    EIndexFormat IndexFormat;
};

/**
 * A static mesh
 * Contains data for rendering a mesh that will never change
 */
class CStaticMesh
{
    friend class CStaticMeshRenderData;

public:
    void UpdateData(const std::vector<SStaticMeshVertex>& InVertices,
        const std::vector<uint32_t>& InIndices);

    CStaticMeshRenderData* GetRenderData() const { return RenderData.get(); }
    const uint32_t& GetIndexCount() const { return IndexCount; }
private:
    std::vector<SStaticMeshVertex> Vertices;
    std::vector<uint32_t> Indices;
    std::unique_ptr<CStaticMeshRenderData> RenderData;
    uint32_t IndexCount;
};

} /* namespace ZE */