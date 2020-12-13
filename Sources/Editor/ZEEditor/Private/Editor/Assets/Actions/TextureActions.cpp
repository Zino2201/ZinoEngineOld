#include "Editor/Assets/TextureActions.h"
#include "Editor/Widgets/TextureEditorTab.h"
#include "Editor/ZEEditor.h"
#include "Reflection/Cast.h"

namespace ze::editor
{

void TextureActions::open_editor(Asset* in_asset,
	const std::shared_ptr<assetmanager::AssetRequestHandle>& in_handle)
{
	if(EditorApp::get().has_tab(in_asset->get_path().string()))
		return;

	OwnerPtr<TextureEditorTab> tab = new TextureEditorTab(
		reflection::cast<Texture>(in_asset), in_handle);
	EditorApp::get().add_tab(tab);
}

}