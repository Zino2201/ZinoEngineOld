#pragma once

#include "ZEUI/Primitives/DockableTab.h"
#include "ZEUI/Render/Font.h"

namespace ze::ui
{

class HorizontalContainer;
class VerticalContainer;
class ScrollableContainer;

}

namespace ze::editor
{

class ZEUIAssetExplorer : public ui::DockableTab
{
public:
	ZEUIAssetExplorer();

	void construct() override;
private:
	ui::VerticalContainer* project_hierarchy;
	ui::VerticalContainer* allo;
	ui::HorizontalContainer* asset_list;
	std::unique_ptr<ui::Font> test;
};

}