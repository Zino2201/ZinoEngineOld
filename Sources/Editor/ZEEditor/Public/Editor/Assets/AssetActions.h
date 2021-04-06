#pragma once

#include "Assets/AssetManager.h"
#include "AssetActions.gen.h"

namespace ze::editor
{

/**
 * Base class for asset actions classes
 * Asset actions classes manages actions that are associated with assets classes
 */
ZCLASS()
class AssetActions
{
	ZE_REFL_BODY()

public:
	/**
	 * Open the specified asset editor of the class
	 */
	virtual void open_editor(Asset* in_asset, 
		const assetmanager::AssetRequestPtr& in_request_handle) = 0;
	virtual const reflection::Class* get_supported_class() const = 0;
};

void initialize_asset_actions_mgr();
void destroy_asset_actions_mgr();
AssetActions* get_actions_for(const reflection::Class* in_class);

}