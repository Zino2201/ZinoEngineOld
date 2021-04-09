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

void Text::compute_desired_size(maths::Vector2f available_size)
{
	desired_size = DrawCommandPrimitiveText::measure(align_mode_.get(), cached_buffer, font_);
}

void Text::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	context.add(new DrawCommandPrimitiveText(font_, cached_buffer, text_.get().size(), align_mode_.get(),
		font_data), arranged_rect.absolute_position, arranged_rect.size);
}

void Text::cache_layout()
{
	if(!hb_font)
		return;

	if(!cached_buffer)
		cached_buffer = hb_buffer_create();
	else
		hb_buffer_reset(cached_buffer);

	hb_buffer_add_utf8(cached_buffer, text_.get().c_str(), -1, 0, -1);
	hb_buffer_set_direction(cached_buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(cached_buffer, HB_SCRIPT_LATIN);
	hb_buffer_set_language(cached_buffer, hb_language_from_string("en", -1));
	hb_buffer_guess_segment_properties(cached_buffer);
	hb_shape(hb_font, cached_buffer, nullptr, 0);

	notify_size_changed();
}

void Text::update_font()
{
	if(hb_font)
		hb_font_destroy(hb_font);

	if(cached_face)
		hb_face_destroy(cached_face);

	hb_blob_t* blob = hb_blob_create(reinterpret_cast<const char*>(font_.font->get_font_data().data()), 
		font_.font->get_font_data().size(), 
		HB_MEMORY_MODE_READONLY,
		nullptr, 
		nullptr);
	cached_face = hb_face_create(blob, 0);
	hb_font = hb_font_create(cached_face);
	font_data.update(font_.size / font_.font->get_em_size() * font_.font->get_sdf_pixel_range());
}

Text* Text::font(const FontInfo& in_font) 
{ 
	font_ = in_font; 

	update_font();
	cache_layout(); 
	
	return this; 
}

}