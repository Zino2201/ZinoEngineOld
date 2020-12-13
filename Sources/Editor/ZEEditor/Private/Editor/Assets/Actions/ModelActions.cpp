#include "Editor/Assets/Actions/ModelActions.h"
#include "Editor/ZEEditor.h"
#include "Reflection/Cast.h"
#include "Editor/Widgets/ModelEditorTab.h"

namespace ze::editor
{

void ModelActions::open_editor(Asset* in_asset,
	const std::shared_ptr<assetmanager::AssetRequestHandle>& in_handle)
{
	if(EditorApp::get().has_tab(in_asset->get_path().string()))
		return;

	OwnerPtr<ModelEditorTab> tab = new ModelEditorTab(
		reflection::cast<Model>(in_asset), in_handle);
	EditorApp::get().add_tab(tab);
}

}