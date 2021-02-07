#include "ZEUI/CompositeWidget.h"

namespace ze::ui
{

void CompositeWidget::compute_desired_size(const maths::Vector2f& in_available_size)
{
	content_.get().compute_desired_size(in_available_size);
}

void CompositeWidget::arrange_children()
{
	content_.get().arrange_children();
}

void CompositeWidget::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	if(content_.is_valid())
		content_.get().paint(renderer, context);
}

}