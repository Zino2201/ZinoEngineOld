#pragma once

#include "ZEUI/CompositeWidget.h"

namespace ze::ui
{

/**
 * A tab that can be docked onto a DockSpace
 */
class DockableTab : public CompositeWidget
{
public:
	void construct() override;

	DockableTab* title(const std::string& in_title) { title_ = in_title; return this; }
private:
	std::string title_;
};

}