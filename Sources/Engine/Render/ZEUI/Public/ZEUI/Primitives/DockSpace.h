#pragma once

#include "ZEUI/Container.h"

namespace ze::ui
{

/**
 * A container that can host DockableTabs
 */
class DockSpace : public Container
{
public:
	using Item = ContainerItem;

	DockSpace(const std::string& in_name) : name(in_name) {}
private:
	std::string name;
};

}