#include "Editor/PropertyEditor.h"
#include <robin_hood.h>
#include "Reflection/Type.h"
#include "Reflection/Enum.h"
#include "ImGui/ImGui.h"
#include "Reflection/Macros.h"

namespace ze::editor
{

class EnumPropertyEditor final : public PropertyEditor
{
public:
	void draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params) override
	{
		ZE_CHECK(e);
		uint64_t& current_value = *reinterpret_cast<uint64_t*>(in_value);

		ImGui::PushID(in_label);

		if(ImGui::BeginCombo(in_label, e->get_value_name(current_value).c_str()))
		{
			for(const auto& [name, value] : e->get_values())
			{
				if(ImGui::Selectable(name.c_str()))
				{
					memcpy(in_value, reinterpret_cast<uint64_t*>(value.get_value_ptr()), 
						e->get_size());
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	const reflection::Enum* e;
};

EnumPropertyEditor enum_property_ed;
robin_hood::unordered_map<const reflection::Type*, std::unique_ptr<PropertyEditor>> editors;

void register_property_editor(const reflection::Type* in_type, OwnerPtr<PropertyEditor> in_editor)
{
	editors.insert({ in_type, in_editor });
}

PropertyEditor* get_property_edtior(const reflection::Type* in_type)
{
	if(in_type->is_enum())
	{
		enum_property_ed.e = static_cast<const reflection::Enum*>(in_type);
		return &enum_property_ed;
	}

	auto editor = editors.find(in_type);
	if(editor != editors.end())
		return editor->second.get();

	return nullptr;
}

}