#pragma once

#include "Assets/AssetCooker.h"
#include "Gfx/Gfx.h"
#include "TextureCooker.gen.h"

namespace ze { class Texture; }

namespace ze::editor
{

ZCLASS()
class TextureCooker final : public AssetCooker
{
	ZE_REFL_BODY()

public:
	TextureCooker();

	void cook(AssetCookingContext& in_context) override;
private:
	/** Cook for editor. Will load from asset datacache */
	void cook_for_editor(AssetCookingContext& in_context);

	gfx::Format get_adequate_format(Texture* in_texture) const;
};

}