#include "EntityProperties.h"
#include "Editor/MapEditor.h"
#include "ImGui/ImGui.h"
#include "Engine/World.h"
#include "Engine/ECS/ComponentManager.h"
#include "Editor/PropertyEditor.h"
#include <imgui_internal.h>
#include "Reflection/Class.h"

namespace ze::editor
{

CEntityProperties::CEntityProperties(CMapEditor& InMapEditor) : MapEditor(InMapEditor) {}

void CEntityProperties::Draw()
{
	if(!ImGui::Begin("Entity Properties") || selected_entity == nullptr)
	{
		ImGui::End();
		return;
	}

	ImGui::PushID(selected_entity.id);

	ImGui::Text("Selected entity: %d", selected_entity.id);
	ImGui::SameLine(0.f, ImGui::GetContentRegionAvail().x - 270);
	ImGui::SetNextItemWidth(150);
	if(ImGui::BeginCombo("##AddComponent", "Add component"))
	{
		for(const auto& type : reflection::Class::get_derived_classes_from(reflection::Class::get<Component>()))
		{
			if(MapEditor.get_world().get_component_mgr().has_component(selected_entity, type))
				continue;

			if(ImGui::Selectable(type->get_name().c_str(), false, ImGuiSelectableFlags_None))
			{
				MapEditor.get_world().get_component_mgr().add_component(selected_entity, type);
			}
		}
		ImGui::EndCombo();
	}

	auto archetype = MapEditor.get_world().get_component_mgr().get_archetype(selected_entity);
	if(archetype)
	{
		for(const auto& type : archetype->id.classes)
		{
			if(type->get_class_flags() & reflection::ClassFlagBits::HideInEditor)
				continue;

			void* component = MapEditor.get_world().get_component_mgr().get_component(selected_entity, type);

			if(ImGui::CollapsingHeader(type->get_name().c_str(), 
				ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
				ImGuiTreeNodeFlags_OpenOnArrow))
			{
				ImGui::SameLine(0.f, ImGui::GetContentRegionAvail().x - 485);
				ImGui::SetNextItemWidth(100);
				ImGui::PushID(type->get_name().c_str());
				if(ImGui::Button("REMOVE"))
				{
					MapEditor.get_world().get_component_mgr().remove_component(selected_entity, type);
				}
				ImGui::PopID();
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 80.f);
				for(const auto& property : type->get_propreties())
				{
					if(property.get_flags() & reflection::PropertyFlagBits::Visible ||
						property.get_flags() & reflection::PropertyFlagBits::Editable)
					{
						if(PropertyEditor* ed = get_property_edtior(property.get_type()))
						{
							if(property.get_flags() & reflection::PropertyFlagBits::Visible)
							{
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}

							ImGui::Text("%s", property.get_name().c_str());
							if(ImGui::IsItemHovered())
								ImGui::SetTooltip(property.get_type()->get_name().c_str());
							ImGui::NextColumn();
							std::string unique_label = "##" + std::to_string(selected_entity.id) + property.get_name();
							ed->draw(unique_label.c_str(), property.get_value_ptr(component));
							ImGui::NextColumn();
							if(property.get_flags() & reflection::PropertyFlagBits::Visible)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}
						}
					}
				}
				ImGui::Columns();
			}

		}

		ImGui::Separator();
		ImGui::Text("Hidden components");
		for(const auto& type : archetype->id.classes)
		{
			if(!(type->get_class_flags() & reflection::ClassFlagBits::HideInEditor))
				continue;

			ImGui::Text(type->get_name().c_str());
			ImGui::SameLine();
			if(ImGui::Button("Remove"))
			{
				MapEditor.get_world().get_component_mgr().remove_component(selected_entity, type);
			}
		}
	}
	else
	{
		ImGui::Text("This entity doesn't have any components.");
	}

	ImGui::PopID();

	ImGui::End();
}

}