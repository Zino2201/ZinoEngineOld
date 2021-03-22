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

void Container::compute_desired_size(maths::Vector2f available_size)
{
	desired_size = maths::Vector2f(0.f);
	maths::Vector2f available_container_container = available_size;

	for(const auto& child : items)
	{
		child->get().compute_desired_size(available_container_container);
		desired_size += child->get().get_desired_size();
		available_container_container -= child->get().get_desired_size();
	}
}

void Container::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	for(const auto& child : items)
		child->get().paint(renderer, context);
}

}