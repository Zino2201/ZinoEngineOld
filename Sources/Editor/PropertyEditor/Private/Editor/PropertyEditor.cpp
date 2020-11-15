#include "Editor/PropertyEditor.h"
#include <robin_hood.h>

namespace ze::editor
{

robin_hood::unordered_map<const reflection::Type*, std::unique_ptr<PropertyEditor>> editors;

void register_property_editor(const reflection::Type* in_type, OwnerPtr<PropertyEditor> in_editor)
{
	editors.insert({ in_type, in_editor });
}

PropertyEditor* get_property_edtior(const reflection::Type* in_type)
{
	auto editor = editors.find(in_type);
	if(editor != editors.end())
		return editor->second.get();

	return nullptr;
}

}