#include "ZEUI/Container.h"
#include "ZEUI/Item.h"

namespace ze::ui
{

void Container::add(ContainerItem* in_item)
{
	ZE_CHECK(in_item->is_valid());

	if(!in_item->get_owner())
	{
		in_item->set_owner(this);
		in_item->get().set_parent(this);
	}

	in_item->get().construct();
	items.emplace_back(in_item);
}

}