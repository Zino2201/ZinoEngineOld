#include "editor/windows/assets/EffectEditor.h"
#include "engine/assets/Effect.h"
#include "misc/cpp/imgui_stdlib.h"

namespace ze::editor
{

EffectEditor::EffectEditor(Asset* in_asset, const assetmanager::AssetRequestPtr& in_request_handle)
	: Window(in_asset->get_path().stem().string(),
		WindowFlagBits::Transient | WindowFlagBits::DockToMainDockSpaceOnce,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
		effect(static_cast<Effect*>(in_asset)), asset_request(in_request_handle), prop_ed(in_asset->get_class(), in_asset)
{

}

void EffectEditor::pre_draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
}

void EffectEditor::post_draw()
{
	ImGui::PopStyleVar();
}

void EffectEditor::draw()
{
	if(ImGui::BeginTable("##effect_tbl", 2, ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		static std::array<char, 2048> buffer = {};
		ImGui::InputTextMultiline("##effect_hlsl", &effect->get_source(), ImVec2(-1, -1));
		ImGui::TableNextColumn();
		ImGui::Dummy(ImVec2(500, 1));
		prop_ed.draw();
		ImGui::EndTable();
	}
}

}