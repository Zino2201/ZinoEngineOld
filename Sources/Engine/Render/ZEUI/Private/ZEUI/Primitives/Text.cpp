#include "ZEUI/Primitives/Text.h"
#include "ImGui/ImGui.h"
#include "ZEUI/Render/Renderer.h"
#include "ZEUI/Render/DrawCommandRect.h"
#include "ZEUI/Render/DrawCommandText.h"
#include "ZEUI/ZEUI.h"
#include <hb.h>

namespace ze::ui
{

Text::~Text() = default;

void Text::construct()
{
	cache_layout();
}

void Text::compute_desired_size(const maths::Vector2f& available_size)
{
	ImVec2 im_size = ImVec2(20, 10); //ImGui::CalcTextSize(text_.c_str());
	desired_size = maths::Vector2f(im_size.x, im_size.y);
}

void Text::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	//context.add<DrawCommandPrimitiveRect>(arranged_rect.absolute_position, arranged_rect.size);
	//OwnerPtr<DrawCommandPrimitiveText> primitive = new DrawCommandPrimitiveText(); 
	//context.add(primitive, arranged_rect.absolute_position, arranged_rect.size);
}

void Text::cache_layout()
{
	if(!hb_font)
		return;

	if(!cached_buffer)
		cached_buffer = hb_buffer_create();
	else
		hb_buffer_clear_contents(cached_buffer);

	hb_buffer_add_utf8(cached_buffer, text_.c_str(), -1, 0, -1);
	hb_buffer_set_direction(cached_buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(cached_buffer, HB_SCRIPT_LATIN);
	hb_buffer_set_language(cached_buffer, hb_language_from_string("en", -1));
	hb_shape(hb_font, cached_buffer, nullptr, 0);
}

void Text::update_font()
{
	if(hb_font)
		hb_font_destroy(hb_font);

	if(cached_face)
		hb_face_destroy(cached_face);

	hb_blob_t* blob = hb_blob_create_from_file("Assets/Fonts/Roboto-Regular.ttf");
	cached_face = hb_face_create(blob, 0);
	hb_font = hb_font_create(cached_face);
}

Text* Text::font(const FontInfo& in_font) 
{ 
	font_ = in_font; 

	update_font();
	cache_layout(); 
	
	return this; 
}

}