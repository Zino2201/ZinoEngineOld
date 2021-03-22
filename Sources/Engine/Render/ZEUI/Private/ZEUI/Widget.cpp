#include "ZEUI/Widget.h"

namespace ze::ui
{

Widget::Widget() : parent(nullptr), size_changed(true), children_arrangement_changed(true) {} 

void Widget::notify_size_changed()
{
	size_changed = true;
	if(parent)
		parent->notify_size_changed();
}

void Widget::construct()
{
	
}
	
void Widget::paint(Renderer& renderer, DrawContext& context)
{
	if(size_changed)
	{
		compute_desired_size(desired_size);
	}

	if(children_arrangement_changed || size_changed)
	{
		arrange_children();
		children_arrangement_changed = false;
		size_changed = false;
	}
}
	
}