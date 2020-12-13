#pragma once

#include "Assets/AssetManager.h"
#include "AssetActions.gen.h"

//namespace ze { class Asset; }
//namespace ze::assetmanager { class AssetRequestHandle; }

namespace ze::editor
{

/**
 * Base class for asset actions classes
 * Manage available actions for specific type of asset 
 */
ZCLASS()
class ASSETACTIONS_API AssetActions
{
	ZE_REFL_BODY()

public:
	virtual void open_editor(Asset* in_asset, 
		const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle) = 0;
	virtual const reflection::Class* get_supported_class() const = 0;
};

ASSETACTIONS_API void initialize_asset_actions_mgr();
ASSETACTIONS_API void destroy_asset_actions_mgr();
ASSETACTIONS_API AssetActions* get_actions_for(const reflection::Class* in_class);

}