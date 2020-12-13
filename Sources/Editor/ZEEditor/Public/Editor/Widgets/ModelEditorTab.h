#pragma once

#include "Assets/AssetManager.h"
#include "Engine/Assets/Model.h"
#include "Tab.h"

namespace ze::editor
{

class ModelEditorTab final : public Tab
{
public:
	ModelEditorTab(ze::Model* in_model, 
		const std::shared_ptr<assetmanager::AssetRequestHandle>& in_request_handle);

	void draw() override;
	std::string get_name() const override { return model->get_path().string(); }
private:
	ze::Model* model;
	std::shared_ptr<assetmanager::AssetRequestHandle> request_handle;
};

}