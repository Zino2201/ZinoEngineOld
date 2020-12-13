#pragma once

#include "Assets/AssetManager.h"
#include "Engine/Assets/Texture.h"
#include "Tab.h"

namespace ze::editor
{

class TextureEditorTab final : public Tab
{
public:
	TextureEditorTab(ze::Texture* in_texture, 
		const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle);

	void draw() override;
	std::string get_name() const override { return texture->get_path().string(); }
private:
	ze::Texture* texture;
	std::shared_ptr<assetmanager::AssetRequestHandle> request_handle;
	size_t mip_level;
};

}