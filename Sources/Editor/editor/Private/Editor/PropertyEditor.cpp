#include "editor/PropertyEditor.h"
#include <robin_hood.h>
#include "imgui/ImGui.h"
#include "reflection/Enum.h"
#include "reflection/Any.h"
#include "reflection/Macros.h"

namespace ze::editor
{

class EnumPropertyEditor final : public PropertyEditor
{
public:
	const reflection::Enum* e;

	bool draw(const char* in_label, void* in_property) override
	{
		ZE_CHECK(e);
		uint64_t& current_value = *reinterpret_cast<uint64_t*>(in_property);

		ImGui::PushID(in_label);

		bool changed = false;

		std::string n = e->get_value_name(reflection::Any(current_value)).c_str();
		if(ImGui::BeginCombo(in_label, n.c_str()))
		{
			for(const auto& [name, value] : e->get_values())
			{
				if(ImGui::Selectable(name.c_str()))
				{
					memcpy(in_property, reinterpret_cast<uint64_t*>(value.get_value_ptr()), e->get_size());
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();

		return changed;
	}
};

EnumPropertyEditor enum_editor;
robin_hood::unordered_map<const reflection::Type*, std::unique_ptr<PropertyEditor>> property_editors;

void register_property_editor(const reflection::Type* in_type, OwnerPtr<PropertyEditor> in_editor)
{
	property_editors.insert({ in_type, in_editor });
}

PropertyEditor* get_property_editor(const reflection::Type* in_type)
{
	if(in_type->is_enum())
	{
		enum_editor.e = static_cast<const reflection::Enum*>(in_type);
		return &enum_editor;
	}

	return property_editors[in_type].get();
}

}