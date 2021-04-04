#pragma once

#include "EngineCore.h"
#include "Window.h"

namespace ze::editor
{

class AssetExplorer : public Window
{
public:
	AssetExplorer() : Window("Asset Explorer") {}
	void draw() override;
};

}