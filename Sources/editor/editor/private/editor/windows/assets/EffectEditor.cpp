#include "editor/windows/assets/EffectEditor.h"
#include "engine/assets/Effect.h"
#include "misc/cpp/imgui_stdlib.h"

namespace ze::editor
{

EffectEditor::EffectEditor(Effect* in_effect, const assetmanager::AssetRequestPtr& in_request_handle)
	: AssetEditor(in_effect, in_request_handle,
		in_effect->get_path().stem().string(),
		WindowFlagBits::Transient | WindowFlagBits::DockToMainDockSpaceOnce | WindowFlagBits::Document,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
		prop_ed(asset->get_class(), asset)
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
		if(ImGui::InputTextMultiline("##effect_hlsl", &asset->get_source(), ImVec2(-1, -1)))
		{
			mark_as_unsaved();
		}
		ImGui::TableNextColumn();
		ImGui::Dummy(ImVec2(500, 1));
		prop_ed.draw();
		ImGui::EndTable();
	}
}

}