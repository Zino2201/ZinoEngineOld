#pragma once

#include "ZEUI/Primitives/DockableTab.h"

namespace ze::editor
{

class EntityList : public ui::DockableTab
{
public:
	void construct() override;
};

}