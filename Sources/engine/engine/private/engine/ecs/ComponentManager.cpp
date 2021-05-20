#include "engine/ecs/ComponentManager.h"
#include <cstring>

namespace ze
{

ComponentManager::ComponentManager()
{

}

void ComponentManager::register_entity(Entity in_entity)
{
	entity_data_map.insert({ in_entity, EntityData() });
}

void ComponentManager::unregister_entity(Entity in_entity)
{

}

void* ComponentManager::add_component(Entity in_entity, const reflection::Class* in_class, bool in_instantiate)
{
	ZE_CHECKF(entity_data_map.contains(in_entity), "Entity not registered");
	ZE_CHECK(in_class);

	EntityData& entity_data = entity_data_map[in_entity];
	EntityArchetype* old_archetype = nullptr;
	if(archetypes.size() > entity_data.archetype_idx)
		old_archetype = archetypes[entity_data.archetype_idx].get();

	/** Create the new archetype */
	EntityArchetype* new_archetype = nullptr;
	{
		EntityArchetypeId new_id;
		if(old_archetype)
			new_id.append(old_archetype->id);
		new_id.add(in_class);
		auto [new_archetype_ptr, idx] = get_or_create_archetype(new_id);
		new_archetype = new_archetype_ptr;
	}

	size_t dst_chunk_idx = get_free_chunk(*new_archetype);
	EntityArchetypeChunk& dst_chunk = new_archetype->chunks[dst_chunk_idx];
	size_t dst_component_type_idx = new_archetype->class_to_chunk_type_idx[in_class];
	dst_chunk.data[dst_component_type_idx].size++;
	size_t dst_component_idx = dst_chunk.data[dst_component_type_idx].get_end_idx();

	if(old_archetype)
	{
		/** Move old data before instantiating the new component */
		EntityArchetypeChunk& src_chunk = old_archetype->chunks[entity_data.chunk_idx];
		
		old_archetype->entities.erase(std::find(old_archetype->entities.begin(), old_archetype->entities.end(), in_entity));
		
		for(const auto& type : old_archetype->id.classes)
		{
			dst_chunk.data[new_archetype->class_to_chunk_type_idx[type]].size++;

			move_component_data(src_chunk, dst_chunk,
				old_archetype->class_to_chunk_type_idx[type],
				new_archetype->class_to_chunk_type_idx[type],
				entity_data.components_idx,
				dst_component_idx);
		
			/** If a hole was created, fill hole */
			if(entity_data.components_idx == src_chunk.data[old_archetype->class_to_chunk_type_idx[type]].size - 1)
			{
				move_component_data(src_chunk, src_chunk,
					old_archetype->class_to_chunk_type_idx[type],
					old_archetype->class_to_chunk_type_idx[type],
					src_chunk.data[old_archetype->class_to_chunk_type_idx[type]].size - 1,
					entity_data.components_idx);
			}

			src_chunk.data[old_archetype->class_to_chunk_type_idx[type]].size--;
		}
	}
	
	void* out_data = dst_chunk.data[dst_component_type_idx].get_end();
	if(in_instantiate)
	{
		in_class->placement_new(out_data);
	}

	entity_data.archetype_idx = new_archetype->idx;
	entity_data.chunk_idx = dst_chunk_idx;
	entity_data.components_idx = dst_component_idx;

	new_archetype->entities.emplace_back(in_entity);

	if(dst_chunk.is_full())
		new_archetype->free_chunk = dst_chunk.next_free_chunk;

	return out_data;
}

void ComponentManager::remove_component(Entity in_entity, const reflection::Class* in_class)
{
	ZE_CHECKF(entity_data_map.contains(in_entity), "Entity not registered");

	EntityData& entity_data = entity_data_map[in_entity];
	EntityArchetype* old_archetype = nullptr;
	if(archetypes.size() > entity_data.archetype_idx)
		old_archetype = archetypes[entity_data.archetype_idx].get();

	/** Create the new archetype */
	EntityArchetype* new_archetype = nullptr;
	{
		EntityArchetypeId new_id = old_archetype->id;
		new_id.remove(in_class);
		auto [new_archetype_ptr, idx] = get_or_create_archetype(new_id);
		new_archetype = new_archetype_ptr;
	}

	size_t src_chunk_idx = entity_data.chunk_idx;
	EntityArchetypeChunk& src_chunk = old_archetype->chunks[entity_data.chunk_idx];
	old_archetype->entities.erase(std::find(old_archetype->entities.begin(), old_archetype->entities.end(), in_entity));
	
	if(new_archetype)
	{
		/** Move the existing data minus the data of the component we remove */
		size_t dst_chunk_idx = get_free_chunk(*new_archetype);
		EntityArchetypeChunk& dst_chunk = new_archetype->chunks[dst_chunk_idx];
		size_t dst_component_idx = dst_chunk.data[0].size;
		new_archetype->entities.emplace_back(in_entity);
		
		for(const auto& type : new_archetype->id.classes)
		{
			dst_chunk.data[new_archetype->class_to_chunk_type_idx[type]].size++;

			move_component_data(src_chunk, dst_chunk,
				old_archetype->class_to_chunk_type_idx[type],
				new_archetype->class_to_chunk_type_idx[type],
				entity_data.components_idx,
				dst_component_idx);

			/** If a hole was created, fill hole */
			if(entity_data.components_idx == src_chunk.data[old_archetype->class_to_chunk_type_idx[type]].size - 1)
			{
				move_component_data(src_chunk, src_chunk,
					old_archetype->class_to_chunk_type_idx[type],
					old_archetype->class_to_chunk_type_idx[type],
					src_chunk.data[old_archetype->class_to_chunk_type_idx[type]].size - 1,
					entity_data.components_idx);
			}

			src_chunk.data[old_archetype->class_to_chunk_type_idx[type]].size--;
		}

		free_component_data(in_class, src_chunk, 
			old_archetype->class_to_chunk_type_idx[in_class],
			entity_data.components_idx);

		src_chunk.data[old_archetype->class_to_chunk_type_idx[in_class]].size--;

		entity_data.archetype_idx = new_archetype->idx;
		entity_data.chunk_idx = dst_chunk_idx;
		entity_data.components_idx = dst_component_idx;
	}
	else
	{
		/** No components to move, free all of them */
		free_component_data(in_class, src_chunk, 
			old_archetype->class_to_chunk_type_idx[in_class],
			entity_data.components_idx);
		src_chunk.data[old_archetype->class_to_chunk_type_idx[in_class]].size--;
		entity_data.archetype_idx = -1;
	}

	old_archetype->free_chunk = src_chunk_idx;
}

void ComponentManager::move_component_data(EntityArchetypeChunk& in_src, EntityArchetypeChunk& in_dst, 
	size_t in_src_component_type_idx, 
	size_t in_dst_component_type_idx, 
	size_t in_src_component_idx,
	size_t in_dst_component_idx)
{
	ZE_CHECK(in_src.data.size() > in_src_component_type_idx);
	ZE_CHECK(in_dst.data.size() > in_dst_component_type_idx);
	ZE_CHECK(in_src.data[in_src_component_type_idx].size > in_src_component_idx);
	ZE_CHECK(in_dst.data[in_dst_component_type_idx].size > in_dst_component_idx);

	memmove(in_dst.data[in_dst_component_type_idx].at(in_dst_component_idx),
		in_src.data[in_src_component_type_idx].at(in_src_component_idx),
		in_src.data[in_src_component_type_idx].data_size);
}

void ComponentManager::free_component_data(const reflection::Class* in_class, EntityArchetypeChunk& in_chunk,
		size_t in_component_type_idx,
		size_t in_component_idx)
{
	ZE_CHECK(in_chunk.data.size() > in_component_type_idx);
	ZE_CHECK(in_chunk.data[in_component_type_idx].size > in_component_idx);

	void* data = in_chunk.data[in_component_type_idx].at(in_component_idx);
	in_class->destructor(data);
}

size_t ComponentManager::get_free_chunk(EntityArchetype& in_archetype)
{
	if(in_archetype.free_chunk == -1)
	{
		in_archetype.chunks.emplace_back();
		
		/** Allocate empty vectors for components data */
		in_archetype.chunks.back().data.resize(in_archetype.id.classes.size());
		in_archetype.free_chunk = in_archetype.chunks.size() - 1;

		/** Allocate empty vectors for components data */
		in_archetype.chunks.back().data.resize(in_archetype.id.classes.size());
	
		size_t i = 0;
		for(const auto& type : in_archetype.id.classes)
		{
			in_archetype.class_to_chunk_type_idx.insert({ type, i});
			in_archetype.chunks.back().data[i].data_size = type->get_size();
			i++;
		}
	}

	return in_archetype.free_chunk;
}

void* ComponentManager::get_component(Entity in_entity, const reflection::Class* in_class)
{
	ZE_CHECKF(entity_data_map.contains(in_entity), "Entity not registered");

	EntityData& data = entity_data_map[in_entity];
	EntityArchetype* archetype = get_archetype(in_entity);
	ZE_CHECK(archetype);
	if(archetype)
	{
		return archetype->chunks[data.chunk_idx].data[archetype->class_to_chunk_type_idx[in_class]].at(data.components_idx);
	}

	return nullptr;
}

bool ComponentManager::has_component(Entity in_entity, const reflection::Class* in_class)
{
	ZE_CHECKF(entity_data_map.contains(in_entity), "Entity not registered");
	EntityArchetype* archetype = get_archetype(in_entity);
	if(archetype)
		return archetype->id.has(in_class);
	
	return false;
}

std::pair<EntityArchetype*, size_t> ComponentManager::get_or_create_archetype(const EntityArchetypeId& in_id)
{
	if(in_id.classes.empty())
		return { nullptr, -1 };

	for(size_t i = 0; i < archetypes.size(); ++i)
	{
		if(archetypes[i]->id == in_id)
			return { archetypes[i].get(), i };
	}

	archetypes.emplace_back(std::make_unique<EntityArchetype>());
	auto& archetype = archetypes.back();
	archetype->idx = archetypes.size() - 1;
	archetype->id = in_id;
	archetype->free_chunk = get_free_chunk(*archetype);

	return { archetypes.back().get(), archetypes.size() - 1 };
}

EntityArchetype* ComponentManager::get_archetype(Entity in_entity) const
{
	ZE_CHECKF(entity_data_map.contains(in_entity), "Entity not registered");
	auto data = entity_data_map.find(in_entity);
	if(data->second.archetype_idx == EntityArchetypeId::invalid_idx)
		return nullptr;

	return archetypes[data->second.archetype_idx].get();
}

}