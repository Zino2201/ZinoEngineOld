#pragma once

#include "Editor/Windows/Window.h"
#include "Assets/AssetManager.h"

namespace ze::editor
{

/**
 * The texture editor
 */
class TextureEditor : public Window
{
public:
	TextureEditor(Asset* in_asset, 
		const assetmanager::AssetRequestPtr& in_request_handle);
protected:
	void draw();
private:
	Asset* asset;
	assetmanager::AssetRequestPtr asset_request;
};

}