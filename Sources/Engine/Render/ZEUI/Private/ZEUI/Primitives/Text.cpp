#include "ZEUI/Primitives/Text.h"
#include "ImGui/ImGui.h"
#include "ZEUI/Render/Renderer.h"
#include "ZEUI/Render/DrawCommandRect.h"

namespace ze::ui
{

void Text::construct()
{
	
}

void Text::compute_desired_size(const maths::Vector2f& available_size)
{
	ImVec2 im_size = ImGui::CalcTextSize(text_.c_str());
	desired_size = maths::Vector2f(im_size.x, im_size.y);
}

void Text::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	context.add<DrawCommandPrimitiveRect>(arranged_rect.absolute_position, arranged_rect.size);
}

}