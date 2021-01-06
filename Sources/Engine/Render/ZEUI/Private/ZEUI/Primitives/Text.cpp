#include "ZEUI/Primitives/Text.h"
#include "ImGui/ImGui.h"

namespace ze::ui
{

void Text::construct()
{
	if(size.x == 0 && size.y == 0)
	{
		ImVec2 im_size = ImGui::CalcTextSize(text_.c_str());
		size = maths::Vector2f(im_size.x, im_size.y);
	}
}

void Text::paint()
{
	ImGui::Text(text_.c_str());
}
	
}