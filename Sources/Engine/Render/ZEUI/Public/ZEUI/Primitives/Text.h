#pragma once

#include "ZEUI/Widget.h"
#include "ZEUI/Item.h"
#include "ZEUI/Render/Font.h"
#include "Gfx/UniformBuffer.h"
#include "ZEUI/Attribute.h"

struct hb_buffer_t;
struct hb_font_t;
struct hb_face_t;

namespace ze::ui
{

/**
 * A widget displaying text
 */
class ZEUI_API Text final : public Widget
{	
public:
	Text(const std::string& text = "")
		: text_(text), align_mode_(TextAlignMode::Left), cached_buffer(nullptr), cached_face(nullptr), hb_font(nullptr) {}
	~Text();

	Text* text(const std::string& in_text) { text_.set(in_text); cache_layout(); return this; }
	Text* align_mode(const TextAlignMode in_align_mode) { align_mode_.set(in_align_mode); return this; }
	Text* font(const FontInfo& in_font);

	void construct() override;
	void paint(Renderer& renderer, DrawContext& context) override;

	void compute_desired_size(maths::Vector2f available_size) override;
private:
	/**
	 * Cache internal text layout using HarfBuzz
	 */
	void cache_layout();
	void update_font();
private:
	Attribute<std::string> text_;
	FontInfo font_;
	Attribute<TextAlignMode> align_mode_;
	hb_buffer_t* cached_buffer;
	hb_face_t* cached_face;
	hb_font_t* hb_font;
	gfx::UniformBuffer<float> font_data;
};
	
}