#include "editor/assets/actions/EffectActions.h"
#include "engine/assets/Effect.h"
#include "editor/windows/assets/EffectEditor.h"
#include "editor/ZEEditor.h"

namespace ze::editor
{

void EffectActions::open_editor(Asset* in_asset,
	const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle)
{
	EditorApp::get().add_window(new EffectEditor(static_cast<Effect*>(in_asset), in_request_handle));
}

}