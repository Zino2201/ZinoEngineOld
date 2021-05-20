#include "editor/propertyeditors/PrimitivesPropertyEditors.h"
#include "imgui/ImGui.h"

namespace ze::editor
{

bool BoolPropertyEditor::draw(const char* in_label, void* in_property)
{
	return ImGui::Checkbox(in_label, reinterpret_cast<bool*>(in_property));
}

}