#include "Editor/Assets/TextureActions.h"
#include "Editor/ZEEditor.h"
#include "Reflection/Cast.h"
#include "Editor/ZEEditor.h"
#include "Editor/Windows/Assets/Texture/TextureEditor.h"

namespace ze::editor
{

void TextureActions::open_editor(Asset* in_asset,
	const assetmanager::AssetRequestPtr& in_handle)
{
	EditorApp::get().add_window(new TextureEditor(in_asset, in_handle));
}

}