#pragma once

#include "ZEUI/Primitives/DockableTab.h"

namespace ze::editor
{

class ZEUIEntityProperties : public ui::DockableTab
{
public:
	void construct() override;
};

}