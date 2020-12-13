#include "Engine/Assets/Model.h"
#include "Gfx/GpuVector.h"
#include "boost/dynamic_bitset.hpp"

namespace ze
{

gfx::GpuVertexBuffer<ModelVertex> vertex_buffer;
boost::dynamic_bitset<uint8_t> vertex_buffer_bitset;

size_t get_free_vertex_idx_or_grow()
{
	for(size_t i = 0; i < vertex_buffer_bitset.size(); ++i)
    {
        if(!vertex_buffer_bitset[i])
            return i;
    }
  
    vertex_buffer.reserve(vertex_buffer.get_capacity() + 1);
    vertex_buffer_bitset.push_back(false);
  
    return vertex_buffer.get_size();
}

static gfx::ResourceHandle get_vertex_buffer()
{
	return vertex_buffer.get_handle();
}

void Model::load_to_gpu_memory()
{
    for(auto& lod : lods)
    {
        lod.vertex_count = lod.vertices.size();
	    lod.index_count = lod.indices.size();


	size_t vertex_idx = get_free_vertex_idx_or_grow();
#if !ZE_WITH_EDITOR

#endif
    }
}

}