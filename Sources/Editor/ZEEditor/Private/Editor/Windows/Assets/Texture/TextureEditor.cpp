#include "Editor/Windows/Assets/Texture/TextureEditor.h"
#include "Assets/Asset.h"
#include <imgui_internal.h>

namespace ze::editor
{

TextureEditor::TextureEditor(Asset* in_asset, 
	const assetmanager::AssetRequestPtr& in_request_handle)
	: Window(in_asset->get_path().filename().replace_extension("").string(),
		WindowFlagBits::Transient | WindowFlagBits::DockToMainDockSpaceOnce),
		asset(in_asset),
		asset_request(in_request_handle)
{
}

void TextureEditor::draw()
{
	ImGui::Columns(2, nullptr, false);
	
	ImGui::Image(0, ImVec2(512, 512));
	
	ImGui::NextColumn();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

	ImGui::TextUnformatted("Texture parameters:");
	
	ImGui::Columns(1);
}

}