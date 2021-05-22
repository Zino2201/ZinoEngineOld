#pragma once

#include "editor/assets/AssetActions.h"
#include "engine/assets/Effect.h"
#include "EffectActions.gen.h"

namespace ze { class Effect; }

namespace ze::editor
{

ZCLASS()
class EffectActions final : public AssetActions
{
	ZE_REFL_BODY()

public:
	void open_editor(Asset* in_asset,
		const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle) override;

	const reflection::Class* get_supported_class() const override { return reflection::Class::get<Effect>(); }
};

}