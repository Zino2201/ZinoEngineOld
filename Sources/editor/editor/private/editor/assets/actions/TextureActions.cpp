#include "editor/assets/TextureActions.h"
#include "editor/ZEEditor.h"
#include "editor/windows/assets/texture/TextureEditor.h"

namespace ze::editor
{

void TextureActions::open_editor(Asset* in_asset,
	const assetmanager::AssetRequestPtr& in_handle)
{
	EditorApp::get().add_window(new TextureEditor(in_asset, in_handle));
}

}