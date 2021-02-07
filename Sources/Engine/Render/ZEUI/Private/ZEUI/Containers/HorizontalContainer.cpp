#include "ZEUI/Containers/HorizontalContainer.h"
#include "ImGui/ImGui.h"
#include "ZEUI/Render/Renderer.h"
#include "ZEUI/Render/DrawCommandRect.h"

namespace ze::ui
{

void HorizontalContainer::compute_desired_size(const maths::Vector2f& available_size)
{
	desired_size = maths::Vector2f(0.f);
	maths::Vector2f available_container_container = available_size;

	for(const auto& child : items)
	{
		child->get().compute_desired_size(available_container_container);
		available_container_container -= child->get().get_desired_size();

		desired_size.x += child->get().get_desired_size().x;
		desired_size.y = std::max(desired_size.y, child->get().get_desired_size().y);
	}
}

void HorizontalContainer::arrange_children()
{
	maths::Vector2f current_pos;
	
	for(const auto& child : items)
	{
		HorizontalContainerItem& item = *static_cast<HorizontalContainerItem*>(child.get());
		
		WidgetRect rect(current_pos, arranged_rect.absolute_position + current_pos, child->get().get_desired_size());

		/** Apply padding */
		rect.absolute_position.x += item.padding_.left;
		rect.position.x += item.padding_.left;

		rect.absolute_position.y += item.padding_.top;
		rect.position.y += item.padding_.top;
		
		rect.size.x += item.padding_.right;
		rect.size.y += item.padding_.bottom;

		item.get().set_arranged_rect(rect);
		item.get().arrange_children();
		current_pos.x += rect.size.x + item.padding_.left;

		if(current_pos.x + rect.size.x > arranged_rect.size.x)
		{
			//current_pos.x = 0;
			//current_pos.y += rect.size.y;
		}
	}
}

}