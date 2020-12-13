#pragma once

#include "Editor/Assets/AssetActions.h"
#include "Engine/Assets/Texture.h"
#include "TextureActions.gen.h"

namespace ze::editor
{

ZCLASS()
class ZEEDITOR_API TextureActions final : public AssetActions
{
	ZE_REFL_BODY()

public:
	void open_editor(Asset* in_asset, 
		const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle) override;

	const reflection::Class* get_supported_class() const override { return reflection::Class::get<Texture>(); }
};

}