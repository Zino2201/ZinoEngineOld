#pragma once

#include "ZEUI/Primitives/DockableTab.h"

namespace ze::editor
{

class EntityProperties : public ui::DockableTab
{
public:
	void construct() override;
};

}