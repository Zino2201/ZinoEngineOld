#pragma once

#include "Editor/Windows/Window.h"
#include "Assets/AssetManager.h"
#include "Editor/PropertiesEditor.h"

namespace ze { class Texture; }

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
	~TextureEditor();
protected:
	void draw();
private:
	Asset* asset;
	assetmanager::AssetRequestPtr asset_request;
	Texture* texture;
	PropertiesEditor properties_editor;
	float zoom;
};

}