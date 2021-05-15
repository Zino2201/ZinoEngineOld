#include "TinyObjFactory.h"
#include <ios>
#include <istream>
#include "Engine/Assets/Model.h"
#include "Serialization/BinaryArchive.h"
#if 0
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#endif
#include "Editor/NotificationSystem.h"

namespace ze::editor
{

TinyObjFactory::TinyObjFactory()
{
	supported_formats = { "obj" };
}

OwnerPtr<Asset> TinyObjFactory::create_from_stream(std::istream& in_stream)
{
	OwnerPtr<Model> model = new Model(1);
#if 0
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &in_stream))
	{
		notification_add(NotificationType::Error, fmt::format("Failed to import obj: {}", err));
		ze::logger::error("Failed to import obj: {}", err);
		return nullptr;
	}

	
	robin_hood::unordered_map<ModelVertex, uint32_t> unique_vertices;

	ModelLod& lod = model->get_lods()[0];
	for(const auto& shape : shapes)
	{
		for(const auto& index : shape.mesh.indices)
		{
			ModelVertex vertex;

			vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];
			
			if(!attrib.texcoords.empty())
			{
				vertex.uv0.x = attrib.texcoords[2 * index.texcoord_index + 0];
				vertex.uv0.y = attrib.texcoords[2 * index.texcoord_index + 1];
			}

			if(!attrib.normals.empty())
			{
				vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
				vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
				vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
			}

			if (unique_vertices.count(vertex) == 0)
			{
				unique_vertices[vertex] = static_cast<uint32_t>(lod.vertices.size());
				lod.vertices.push_back(vertex);
			}

			lod.indices.push_back(unique_vertices[vertex]);
		}
	}
#endif
	return model;
}

}