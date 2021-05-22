#pragma once

#include "editor/windows/Window.h"
#include "assets/AssetManager.h"
#include "gfx/Gfx.h"
#include "editor/PropertiesEditor.h"

namespace ze { class Effect; }

namespace ze::editor
{

class EffectEditor : public Window
{
public:
	EffectEditor(Asset* in_asset,
		const assetmanager::AssetRequestPtr& in_request_handle);
protected:
	void pre_draw() override;
	void draw() override;
	void post_draw() override;
private:
	assetmanager::AssetRequestPtr asset_request;
	Effect* effect;
	PropertiesEditor prop_ed;
};

}