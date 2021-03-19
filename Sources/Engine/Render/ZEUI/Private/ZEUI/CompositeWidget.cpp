#include "ZEUI/CompositeWidget.h"

namespace ze::ui
{

void CompositeWidget::compute_desired_size(const maths::Vector2f& in_available_size)
{
	if(content_.is_valid())
	{
		content_.get().compute_desired_size(in_available_size);
		desired_size = content_.get().get_desired_size();
	}
}

void CompositeWidget::arrange_children()
{
	if(content_.is_valid())
	{
		WidgetRect rect(maths::Vector2f(), arranged_rect.absolute_position, content_.get().get_desired_size());
		content_.get().set_arranged_rect(rect);
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