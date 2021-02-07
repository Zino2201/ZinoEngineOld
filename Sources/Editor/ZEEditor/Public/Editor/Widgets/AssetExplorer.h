#pragma once

#include "ZEUI/Primitives/DockableTab.h"

namespace ze::ui
{

class HorizontalContainer;
class VerticalContainer;
class ScrollableContainer;

}

namespace ze::editor
{

class AssetExplorer : public ui::DockableTab
{
public:
	AssetExplorer();

	void construct() override;
private:
	ui::VerticalContainer* project_hierarchy;
	ui::VerticalContainer* allo;
	ui::HorizontalContainer* asset_list;
};

}