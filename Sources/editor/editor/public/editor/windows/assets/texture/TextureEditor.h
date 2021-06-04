#pragma once

#include "editor/windows/assets/AssetEditor.h"
#include "editor/PropertiesEditor.h"
#include "gfx/Gfx.h"

namespace ze { class Texture; }

namespace ze::editor
{

/**
 * The texture editor
 */
class TextureEditor : public AssetEditor<Texture>
{
public:
	TextureEditor(Texture* in_texture, 
		const assetmanager::AssetRequestPtr& in_request_handle);
	~TextureEditor();
protected:
	void draw() override;
private:
	void on_mipmap_changed(void* in_enabled);
	void build_views();
private:
	PropertiesEditor properties_editor;
	float zoom;
	int current_miplevel;
	bool should_update_resource;
	bool should_update_mipmaps;
	std::vector<gfx::UniqueTextureView> views;
	
};

}