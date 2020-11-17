#include "EntityProfiler.h"
#include "ImGui/ImGui.h"
#include "Editor/MapEditor.h"
#include "Engine/World.h"
#include "Engine/ECS/EntityManager.h"
#include "Engine/ECS/ComponentManager.h"

namespace ze::editor
{

EntityProfiler::EntityProfiler(CMapEditor& in_editor) : map_editor(in_editor) {}

void EntityProfiler::draw()
{
	if(!ImGui::Begin("Entity Profiler"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Entity count: %d", map_editor.get_world().get_entity_mgr().get_entities().size());
	ImGui::Text("Entity archetype count: %d", map_editor.get_world().get_component_mgr().get_archetypes().size());
	
	for(const auto& archetype : map_editor.get_world().get_component_mgr().get_archetypes())
	{
		std::string name;
		bool first = true;
		for(const auto& type : archetype->id.classes)
		{
			if(first)
			{
				name += type->get_name();
				first = false;
			}
			else
				name += " + " + type->get_name();
		}

		bool opened = ImGui::CollapsingHeader(name.c_str(), 
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_OpenOnArrow);
		if(ImGui::IsItemHovered())
			ImGui::SetTooltip(name.c_str());
		if(opened)
		{
			ImGui::Text("Entity count: %d", archetype->entities.size());
			ImGui::Text("Chunk count: %d", archetype->chunks.size());
	
			float total_size = 0;
			for(const auto& type : archetype->id.classes)
				total_size += type->get_size();

			ImGui::Text("Archetype size: %f Kib", total_size / 1024.f);

			/** Chunks */
			ImGui::Separator();
			size_t idx = 0;
			for(const auto& chunk : archetype->chunks)
			{
				ImGui::Text("Chunk %i: %f Kib/%f Kib (%d entities)", 
					idx, (total_size * chunk.data[0].size) / 1024.f, chunk.chunk_data_size / 1024.f,
					chunk.data[0].size);
				idx++;
			}
			ImGui::Separator();
			ImGui::Text("Total size: %f Kib", (archetype->entities.size() * total_size) / 1024.f);
		}
	}

	ImGui::End();
}

}