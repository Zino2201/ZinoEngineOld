#pragma once

#include "ZEUI/Primitives/DockableTab.h"

namespace ze::editor
{

class ZEUIEntityList : public ui::DockableTab
{
public:
	void construct() override;
};

}