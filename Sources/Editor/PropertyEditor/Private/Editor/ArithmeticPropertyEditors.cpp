#include "Editor/ArithmeticPropertyEditors.h"
#include "Reflection/Any.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

bool BoolPropertyEditor::draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params)
{
	return ImGui::Checkbox(in_label, reinterpret_cast<bool*>(in_value));
}

bool Uint32PropertyEditor::draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params)
{
	return ImGui::DragInt(in_label, reinterpret_cast<int*>(in_value));
}

bool FloatPropertyEditor::draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params)
{
	return ImGui::DragFloat(in_label, reinterpret_cast<float*>(in_value));
}

}