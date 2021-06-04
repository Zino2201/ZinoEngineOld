#pragma once

#include "AssetEditor.h"
#include "gfx/Gfx.h"
#include "editor/PropertiesEditor.h"

namespace ze { class Effect; }

namespace ze::editor
{

class EffectEditor : public AssetEditor<Effect>
{
public:
	EffectEditor(Effect* in_effect,
		const assetmanager::AssetRequestPtr& in_request_handle);
protected:
	void pre_draw() override;
	void draw() override;
	void post_draw() override;
private:
	PropertiesEditor prop_ed;
};

}