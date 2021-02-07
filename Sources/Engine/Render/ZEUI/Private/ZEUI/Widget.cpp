#include "ZEUI/Widget.h"

namespace ze::ui
{

Widget::Widget() : size_changed(true), children_arrangement_changed(true) {} 

void Widget::construct()
{
	
}
	
void Widget::paint(Renderer& renderer, DrawContext& context)
{
	if(size_changed)
	{
		compute_desired_size(maths::Vector2f());
	}

	if(children_arrangement_changed || size_changed)
	{
		arrange_children();
		children_arrangement_changed = false;
		size_changed = false;
	}
}
	
}