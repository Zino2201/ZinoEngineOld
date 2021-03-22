#include "ZEUI/CompositeWidget.h"

namespace ze::ui
{

void CompositeWidget::compute_desired_size(maths::Vector2f in_available_size)
{
	if(content_.is_valid())
	{
		content_.get().compute_desired_size(in_available_size);
		if(content_.get_size_mode() != SizeMode::Fill)
			desired_size = content_.get().get_desired_size();
		else
			desired_size = parent->get_desired_size();
	}
}

void CompositeWidget::arrange_children()
{
	if(content_.is_valid())
	{
		maths::Vector2f position;
		maths::Vector2f abs_position = arranged_rect.absolute_position;
		maths::Vector2f my_size = desired_size;
		maths::Vector2f content_size = content_.get().get_desired_size();
		switch(content_.get_horizontal_align_mode())
		{
		case HorizontalAlignMode::Center:
			position.x = (my_size.x / 2) - (content_size.x / 2);
			abs_position.x += (my_size.x / 2) - (content_size.x / 2);
			break;
		case HorizontalAlignMode::Right:
			position.x = my_size.x - content_size.x;
			abs_position.x += my_size.x - content_size.x;
			break;
		}

		content_.get().set_arranged_rect(WidgetRect(position, abs_position, 
			desired_size));

		content_.get().arrange_children();
	}
}

void CompositeWidget::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	if(content_.is_valid())
		content_.get().paint(renderer, context);
}

}