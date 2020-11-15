#include "Editor/VectorPropertyEditor.h"
#include "ImGui/ImGui.h"
#include "Maths/Vector.h"

namespace ze::editor
{

void Vector3fPropertyEditor::draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params)
{
	maths::Vector3f* vec = reinterpret_cast<maths::Vector3f*>(in_value);
	
	ImGui::PushID(in_label);

	ImGui::PushItemWidth(100);
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::DragFloat("##X", &vec->x);
	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vec->y);
	ImGui::SameLine();

	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vec->z);

	ImGui::PopItemWidth();

	ImGui::PopID();
}

void Vector3dPropertyEditor::draw(const char* in_label, void* in_value, PropertyEditorDrawParams in_params)
{
	maths::Vector3d* vec = reinterpret_cast<maths::Vector3d*>(in_value);
	
	ImGui::PushID(in_label);

	ImGui::PushItemWidth(100);

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::DragScalar("##X", ImGuiDataType_Double, &vec->x, 1.f);
	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::DragScalar("##Y", ImGuiDataType_Double, &vec->y, 1.f);
	ImGui::SameLine();

	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::DragScalar("##Z", ImGuiDataType_Double, &vec->z, 1.f);

	ImGui::PopItemWidth();

	ImGui::PopID();
}

}