#pragma once

#include "ECS.h"
#include "Engine/ECS/Component.h"
#include <robin_hood.h>
#include <array>
#include "ComponentManager.gen.h"

namespace ze::reflection { class Class; }

namespace ze
{

struct EntityArchetypeId 
{
	static constexpr size_t invalid_idx = -1;

	std::vector<const reflection::Class*> classes;

	EntityArchetypeId(std::vector<const reflection::Class*> in_classes = {})
	{
		std::sort(in_classes.begin(), in_classes.end());
		classes = std::move(in_classes);
	}

	void add(const reflection::Class* in_class)
	{
		classes.emplace_back(in_class);
		std::sort(classes.begin(), classes.end());
	}

	void append(const EntityArchetypeId& in_other)
	{
		classes.insert(classes.end(), in_other.classes.begin(), in_other.classes.end());
		std::sort(classes.begin(), classes.end());
	}

	void remove(const reflection::Class* in_class)
	{
		classes.erase(std::find(classes.begin(), classes.end(), in_class));
		std::sort(classes.begin(), classes.end());
	}

	size_t find(const reflection::Class* in_class) const
	{
		for(size_t i = 0; i < classes.size(); ++i)
		{
			if(classes[i] == in_class)
				return i;
		}

		return invalid_idx;
	}

	bool has(const reflection::Class* in_class) const
	{
		for(const auto& c : classes)
			if(c == in_class)
				return true;

		return false;
	}

	bool has(const std::vector<const reflection::Class*>& in_classes) const
	{
		for(const auto& c : in_classes)
			if(!has(c))
				return false;

		return true;
	}

	ZE_FORCEINLINE bool operator==(const EntityArchetypeId& other) const { return classes == other.classes; }
};

struct EntityArchetypeChunk
{
	static constexpr uint64_t chunk_data_size = 16384;
	
	struct ComponentData
	{
		size_t data_size;
		size_t size;
		std::array<uint8_t, chunk_data_size> data;

		ZE_FORCEINLINE uint8_t* at(const size_t in_idx)
		{
			return data.data() + (in_idx * data_size);
		}

		ZE_FORCEINLINE uint8_t* get_end()
		{
			return data.data() + ((size - 1) * data_size);
		}

		ZE_FORCEINLINE size_t get_end_idx()
		{
			ZE_CHECK(size != 0);
			return size - 1;
		}
	};

	std::vector<ComponentData> data;
	size_t next_free_chunk = -1;

	ZE_FORCEINLINE bool is_full() const
	{
		size_t total_size = 0;
		for(const auto& d : data)
			total_size += d.data_size;
		return (data[0].size + 1) * total_size > chunk_data_size;
	}
};

struct EntityArchetype
{
	size_t idx;
	EntityArchetypeId id;
	std::vector<Entity> entities;
	std::vector<EntityArchetypeChunk> chunks;
	robin_hood::unordered_map<const reflection::Class*, size_t> class_to_chunk_type_idx;
	size_t free_chunk = -1;
};

/*
 * A component manager
 * Manages all components of a single world
 */
ZCLASS()
class ENGINE_API ComponentManager final
{
	ZE_REFL_BODY()

	struct EntityData
	{
		static constexpr size_t invalid_idx = -1;

		size_t archetype_idx = invalid_idx;
		size_t chunk_idx = invalid_idx;
		size_t components_idx = invalid_idx;
	};

public:
	ComponentManager();

	ComponentManager(const ComponentManager&) = delete;
	void operator=(const ComponentManager&) = delete;

	/**
	 * Register a entity to the component manager system
	 */
	void register_entity(Entity in_entity);

	/**
	 * Unregister a entity
	 * This will free all components attached to the entity
	 */
	void unregister_entity(Entity in_entity);

	EntityArchetype* get_archetype(Entity in_entity) const;

	template<typename T, typename... Args> requires IsComponent<T>
	ZE_FORCEINLINE T& add_component(Entity in_entity, Args&&... in_args)
	{
		const reflection::Class* class_ = reflection::Class::get<T>();
		void* data = add_component(in_entity, class_, false);
		ZE_CHECK(data);
		new (data) T(std::forward<Args>(in_args)...);
		return *reinterpret_cast<T*>(data);
	}

	void* add_component(Entity in_entity, const reflection::Class* in_class, bool in_instantiate = true);
	void remove_component(Entity in_entity, const reflection::Class* in_class);

	void* get_component(Entity in_entity, const reflection::Class* in_class);
	bool has_component(Entity in_entity, const reflection::Class* in_class);
	
	template<typename T> requires IsComponent<T>
	ZE_FORCEINLINE T& get_component(Entity in_entity)
	{
		void* data = get_component(in_entity, reflection::Class::get<T>());
		ZE_CHECK(data);
		return *reinterpret_cast<T*>(data);
	}

	ZE_FORCEINLINE const auto& get_archetypes() const { return archetypes; }
private:
	std::pair<EntityArchetype*, size_t> get_or_create_archetype(const EntityArchetypeId& in_id);
	
	/**
	 * Get a free chunk from this archetype
	 * Will allocate a new chunk if needed
	 */
	size_t get_free_chunk(EntityArchetype& in_archetype);
	
	/**
	 * Move a single component to another chunk
	 * \warning Do not fills holes
	 * \param in_src Source chunk
	 * \param in_dst Destination chunk
	 * \param in_src_component_type_idx Source chunk component type index
	 * \param in_dst_component_type_idx Destination chunk component type index
	 * \param in_src_component_idx Source component idx
	 * \param in_dstcomponent_idx Destination component idx
	 */
	void move_component_data(EntityArchetypeChunk& in_src, EntityArchetypeChunk& in_dst, 
		size_t in_src_component_type_idx, 
		size_t in_dst_component_type_idx, 
		size_t in_src_component_idx,
		size_t in_dst_component_idx);

	/*
	 * Free a single component
	 * \warning Do not fills holes
	 */
	void free_component_data(const reflection::Class* in_class, EntityArchetypeChunk& in_chunk,
		size_t in_component_type_idx,
		size_t in_component_idx);
private:
	std::vector<std::unique_ptr<EntityArchetype>> archetypes;
	robin_hood::unordered_flat_map<Entity, EntityData> entity_data_map;
};

}