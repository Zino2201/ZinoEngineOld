#include "Editor/Windows/Assets/Texture/TextureEditor.h"
#include "Assets/Asset.h"

namespace ze::editor
{

TextureEditor::TextureEditor(Asset* in_asset, 
	const assetmanager::AssetRequestPtr& in_request_handle)
	: Window(asset->get_path().filename().string()),
		asset(in_asset),
		asset_request(in_request_handle)
{
}

void TextureEditor::draw()
{
	ImGui::TextUnformatted("TEXTURE EDITOR WOW!!!!");
}

}