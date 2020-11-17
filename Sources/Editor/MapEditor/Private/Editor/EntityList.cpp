#include "EntityList.h"
#include "ImGui/ImGui.h"
#include "Engine/World.h"
#include "Engine/ECS/EntityManager.h"
#include "Editor/MapEditor.h"

namespace ze::editor
{

CEntityList::CEntityList(World& in_world, CMapEditor& InMapEditor) : world(in_world), 
	MapEditor(InMapEditor) {}

void CEntityList::Draw()
{
	if(!ImGui::Begin("Entity List"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("World %s | %d entities",
		"UNNAMED",
		world.get_entity_mgr().get_entities().size());
	ImGui::Text("Camera Position: %.3f %.3f %.3f", MapEditor.get_cam_pos().x, MapEditor.get_cam_pos().y,
		MapEditor.get_cam_pos().z);

	ImGui::Text("Camera Forward: %.3f %.3f %.3f", MapEditor.get_cam_fwd().x, MapEditor.get_cam_fwd().y,
		MapEditor.get_cam_fwd().z);

	ImGui::Separator();
	
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("New entity"))
		{
			world.spawn_entity();
		}

		if (ImGui::MenuItem("Destroy", nullptr, false, selected_entity != nullptr))
		{
			world.destroy_entity(selected_entity);
			selected_entity = Entity();
			on_entity_selected.execute(Entity());
		}

		ImGui::EndPopup();
	}
	

	std::vector<Entity> entities;
	entities.reserve(world.get_entity_mgr().get_entities().size());
	for (Entity entity : world.get_entity_mgr().get_entities())
		entities.emplace_back(entity);
	std::sort(entities.begin(), entities.end());

	for (Entity entity : entities)
	{
		std::string label = "Entity ID " + std::to_string(entity.id);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAvailWidth;
		if(selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		bool opened = ImGui::TreeNodeEx(label.c_str(), flags);
		if(ImGui::IsItemClicked())
		{
			selected_entity = entity;
			on_entity_selected.execute(Entity(entity));
		}

		if(opened)
		{
			ImGui::Text("ekip"); 
			ImGui::TreePop(); 
		}
	}	
	ImGui::End();
}

}