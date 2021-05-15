#pragma once

#include "Editor/Windows/Window.h"
#include "Assets/AssetManager.h"
#include "Editor/PropertiesEditor.h"
#include "Gfx/Gfx.h"

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
	void on_mipmap_changed(void* in_enabled);
	void build_views();
private:
	assetmanager::AssetRequestPtr asset_request;
	Texture* texture;
	PropertiesEditor properties_editor;
	float zoom;
	int current_miplevel;
	bool should_update_resource;
	bool should_update_mipmaps;
	std::vector<gfx::UniqueTextureView> views;
};

}