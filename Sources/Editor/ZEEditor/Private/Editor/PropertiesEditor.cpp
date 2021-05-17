#include "Editor/PropertiesEditor.h"
#include "ImGui/ImGui.h"
#include "Editor/PropertyEditor.h"
#include <imgui_internal.h>

namespace ze::editor
{

PropertiesEditor::PropertiesEditor(const ze::reflection::Class* in_class, void* in_object)
    : refl_class(in_class), object(in_object)
{
    /** Fill category hashmap */
    for (const auto& property : refl_class->get_properties())
    {
        categories[property.has_metadata("Category") ? property.get_metadata("Category") : "Misc"]
            .properties.emplace_back(&property);
    }

    /** Sort properties */
    for (auto& [name, category] : categories)
    {
        std::sort(category.properties.begin(), category.properties.end(), [](const auto& left, const auto& right) {
            return *left > *right;
        });
    }
}

bool PropertiesEditor::draw()
{
    ImGui::Spacing();

    bool edited = false;

    /** Display reflected properties */
    for (auto& [name, category] : categories)
    {
        ImGui::PushID(name.c_str());

        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("PropertyTable",
                    2,
                    ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 10.f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None, 50.f);
                for (const auto& property : category.properties)
                {
                    if (!property->has_metadata("Visible"))
                        continue;

                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(property->get_name().c_str());
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(property->get_type()->get_name().c_str());
                            ImGui::EndTooltip();
                        }
                    }
                    {
                        ImGui::TableNextColumn();

                        if (!property->has_metadata("Editable"))
                        {
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                        }

                        if (PropertyEditor* editor = get_property_editor(property->get_type()))
                        {
                            std::string unique_label = "##" + property->get_name();
                            if (editor->draw(unique_label.c_str(), property->get_value_ptr(object)))
                            {
                                edited = true;
                                if (on_value_changed_map.count(property->get_name()))
                                {
                                    on_value_changed_map[property->get_name()].execute(property->get_value_ptr(object));
                                }
                            }
                        }
                        else
                        {
                            ImGui::Text("No property editor available for type %s",
                                property->get_type()->get_name().c_str());
                        }

                        if (!property->has_metadata("Editable"))
                        {
                            ImGui::PopItemFlag();
                            ImGui::PopStyleVar();
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::PopID();
    }

    return edited;
}

void PropertiesEditor::bind_to_value_changed(const std::string& in_property, const std::function<void(void*)>& in_func)
{
    on_value_changed_map[in_property].bind(in_func);
}

} // namespace ze::editor