#pragma once

#include "editor/windows/Window.h"
#include "assets/AssetManager.h"
#include "PlatformMgr.h"

namespace ze::editor
{

template<typename AssetClass>
class AssetEditor : public Window
{
public:
	AssetEditor(AssetClass* in_asset, 
		const assetmanager::AssetRequestPtr& in_request_handle,
		const std::string& in_title,
		WindowFlags in_flags = WindowFlags(),
		const int in_imgui_flags = 0) :
			Window(in_title, in_flags | WindowFlagBits::Document | WindowFlagBits::Transient, in_imgui_flags),
			asset(in_asset),
			request_handle(in_request_handle) {}

	void save() override
	{
		assetmanager::save_asset(assetmanager::AssetSaveInfo(asset, get_current_platform()));
	}
	
	AssetClass* get_asset() const { return asset; }
	const assetmanager::AssetRequestPtr& get_request_handle() const { return request_handle; }
protected:
	AssetClass* asset;
	assetmanager::AssetRequestPtr request_handle;
};
	
}