#pragma once

#include "Assets/Asset.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/Pipeline.h"

namespace ZE
{

class CStaticMesh;

/**
 * A static mesh's vertex
 */
struct SStaticMeshVertex
{
    Math::SVector3Float Position;
    Math::SVector3Float Normal;

    bool operator==(const SStaticMeshVertex& InOther) const
    {
        return Position == InOther.Position &&
            Normal == InOther.Normal;
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
			    offsetof(SStaticMeshVertex, Normal)),
		};
	}
};

struct SStaticMeshVertexHash
{
	std::size_t operator()(const SStaticMeshVertex& InVertex) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine<Math::SVector3Float, Math::SVector3FloatHash>(Seed, InVertex.Position);
		HashCombine<Math::SVector3Float, Math::SVector3FloatHash>(Seed, InVertex.Normal);

		return Seed;
	}
};

/**
 * Render data of a static mesh
 */
class CStaticMeshRenderData
{
public:
    CStaticMeshRenderData(CStaticMesh* InStaticMesh);

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
class CStaticMesh : public CAsset
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