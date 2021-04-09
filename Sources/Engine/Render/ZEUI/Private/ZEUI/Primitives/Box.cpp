#include "ZEUI/Primitives/Box.h"
#include "ZEUI/Render/Renderer.h"
#include "ZEUI/Render/DrawCommandRect.h"

namespace ze::ui
{

void Box::paint(Renderer& renderer, DrawContext& context)
{
	switch(brush_.type)
	{
	case Brush::Type::Color:
		context.add(new DrawCommandPrimitiveRect(std::get<Brush::Color>(brush_.info).color), 
			arranged_rect.absolute_position, arranged_rect.size);
		break;
	}

	CompositeWidget::paint(renderer, context);
}

}