#include "Editor/Widgets/ModelEditorTab.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

ModelEditorTab::ModelEditorTab(Model* in_model,
	const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle)
	: model(in_model), request_handle(in_request_handle)
{
}

void ModelEditorTab::draw()
{
	ZE_CHECK(model && request_handle);

	ImGui::PushID(model->get_path().string().c_str());
	{
		ui::SImGuiAutoStyleVar WinPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (!ImGui::BeginChild("TabModelEditor", ImVec2(0, 0), false, ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar))
		{
			ImGui::EndChild();
			ImGui::PopID();
			return;
		}
	}

	ImGui::BeginChild("Model", ImVec2(800, 500), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Image(nullptr, ImVec2(800, 500));
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Properties", ImVec2(800, 500), false, ImGuiWindowFlags_AlwaysAutoResize);

	size_t i = 0;
	for(const auto& lod : model->get_lods())
	{
		std::string header = "LOD " + std::to_string(i);
		if(ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Triangle count: %d", lod.vertex_count / 3);
			ImGui::Text("Vertex count: %d", lod.vertex_count);
		}
	}

	ImGui::Separator();

	for(const auto& property : reflection::Class::get<Model>()->get_properties())
	{
		if(property.has_metadata("Visible"))
		{
			ImGui::Text("%s: %s", property.get_name().c_str(), property.get_value(model).to_string().c_str());	
		}
	}

	ImGui::EndChild();
	ImGui::EndChild();
	ImGui::PopID();
}

}