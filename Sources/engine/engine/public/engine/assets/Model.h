#pragma once

#include "assets/Asset.h"
#include "maths/Vector.h"
#include "serialization/types/Vector.h"
#include "Model.gen.h"

namespace ze
{

/**
 * A single model vertex
 */
struct ModelVertex
{
    enum Version
    {
        Ver0,
    };

	maths::Vector3f position;
    maths::Vector2f uv0;
    maths::Vector2f uv1;
    maths::Vector2f uv2;
    maths::Vector2f uv3;
	maths::Vector3f normal;

    ZE_FORCEINLINE bool operator==(const ModelVertex& other) const
    {
        return position == other.position &&
            uv0 == other.uv0 &&
            uv1 == other.uv1 &&
            uv2 == other.uv2 &&
            uv3 == other.uv3 &&
            normal == other.normal;
    }

    ZE_FORCEINLINE bool operator!=(const ModelVertex& other) const
    {
        return position != other.position ||
            uv0 != other.uv0 ||
            uv1 != other.uv1 ||
            uv2 != other.uv2 ||
            uv3 != other.uv3 ||
            normal != other.normal;
    }

    template<typename ArchiveType>
    void serialize(ArchiveType& in_archive, const uint32_t& in_version)
    {
        in_archive <=> position;
        in_archive <=> uv0;
        in_archive <=> uv1;
        in_archive <=> uv2;
        in_archive <=> uv3;
        in_archive <=> normal;
    }
};
ZE_SERL_TYPE_VERSION(ModelVertex, ModelVertex::Ver0);

/**
 * A view to a vertex buffer
 */
struct ModelVertexView
{
    uint32_t offset;
    uint32_t size;
};

/**
 * A single model LOD
 */
struct ModelLod
{
    static constexpr size_t max_uv_channels = 8;

    enum Version
    {
        Ver0,
    };

    /** Actual vertex, index data. Only available in editor/when keep_in_ram is true */
    std::vector<ModelVertex> vertices;
    std::vector<uint32_t> indices;

    /** Map each material to a view of the vertex buffer */
    std::vector<ModelVertexView> material_views;

    /** Index of the vertex data in the global vertex buffer */
    size_t vertex_buffer_idx;
    
    /** Index of the index data in the global index buffer */
    size_t index_buffer_idx;

    size_t vertex_count;
    size_t index_count;

    ModelLod() : vertex_buffer_idx(0),
        index_buffer_idx(0), vertex_count(0), index_count(0) {}

    template<typename ArchiveType>
    void serialize(ArchiveType& in_archive, const uint32_t& in_version)
    {
        in_archive <=> vertices;
        in_archive <=> indices;
    }
};
ZE_SERL_TYPE_VERSION(ModelLod, ModelLod::Ver0);

ZCLASS()
class ENGINE_API Model : public Asset
{
	ZE_REFL_BODY()

public:
    Model() = default;
    Model(const size_t& in_lod_count)
    {
        lods.resize(in_lod_count);
    }

    template<typename ArchiveType>
    void serialize(ArchiveType& in_archive)
    {
	  
    }

    /**
     * \return Handle to the large vertex buffer array containing all models vertex buffers
     */
    //static gfx::ResourceHandle get_vertex_buffer();

    ZE_FORCEINLINE auto& get_lods() { return lods; }
private:
    void load_to_gpu_memory();
private:
	std::vector<ModelLod> lods;

	/** If true, the Model will be kept in CPU memory (ignored in editor) */
    ZPROPERTY(Editable, Visible)
	bool keep_in_ram;
};

}

namespace std
{

template<> struct hash<ze::ModelVertex>
{
    ZE_FORCEINLINE uint64_t operator()(const ze::ModelVertex& in_vertex) const
    {
        uint64_t hash = 0;
        
        ze::hash_combine(hash, in_vertex.position);
        ze::hash_combine(hash, in_vertex.uv0);
        ze::hash_combine(hash, in_vertex.uv1);
        ze::hash_combine(hash, in_vertex.uv2);
        ze::hash_combine(hash, in_vertex.uv3);
        ze::hash_combine(hash, in_vertex.normal);

        return hash;
    }
};

}