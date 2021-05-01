#include "ZEUI/Containers/VerticalContainer.h"
#include "ImGui/ImGui.h"

namespace ze::ui
{

void VerticalContainer::compute_desired_size(maths::Vector2f available_size)
{
	desired_size = maths::Vector2f(0.f);

	for(const auto& child : items)
	{
		child->get().compute_desired_size(available_size);
		available_size -= child->get().get_desired_size();

		desired_size.x = std::max(desired_size.x, child->get().get_desired_size().x);
		desired_size.y += child->get().get_desired_size().y;
	}
}

void VerticalContainer::arrange_children()
{
	maths::Vector2f current_pos;

	for(const auto& child : items)
	{
		VerticalContainerItem& item = *static_cast<VerticalContainerItem*>(child.get());

		WidgetRect rect(current_pos, arranged_rect.absolute_position + current_pos, child->get().get_desired_size());
		switch (item.halign_mode_.get())
		{
		case HorizontalAlignMode::Left:
			break;
		case HorizontalAlignMode::Center:
			rect.absolute_position.x = (arranged_rect.size.x / 2) - (rect.size.x / 2);
			break;
		case HorizontalAlignMode::Right:
			rect.absolute_position.x = arranged_rect.size.x - rect.size.x;
			break;
		}

		/** Apply padding */
		rect.absolute_position.x += item.padding_.left;
		rect.position.x += item.padding_.left;

		rect.absolute_position.y += item.padding_.top;
		rect.position.y += item.padding_.top;

		rect.size.x += item.padding_.right;
		rect.size.y += item.padding_.bottom;

		item.get().set_arranged_rect(rect);
		item.get().arrange_children();
		current_pos.y += rect.size.y;
	}
}


}