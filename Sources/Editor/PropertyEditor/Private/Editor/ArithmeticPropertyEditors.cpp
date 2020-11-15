#include "Editor/ArithmeticPropertyEditors.h"
#include "Reflection/Any.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

void Uint32PropertyEditor::draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params)
{
	ImGui::DragInt(in_label, reinterpret_cast<int*>(in_value));
}

}