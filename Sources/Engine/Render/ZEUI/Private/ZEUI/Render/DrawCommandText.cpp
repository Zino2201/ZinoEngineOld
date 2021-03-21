#include "ZEUI/Render/DrawCommandText.h"
#include "Gfx/Effect/EffectDatabase.h"
#include "Gfx/Gfx.h"
#include "ImGui/ImGui.h"
#include <hb.h>

namespace ze::ui
{

maths::Vector2f DrawCommandPrimitiveText::measure(hb_buffer_t* text, const FontInfo& in_font)
{
	maths::Vector2f size;

	uint32_t glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(text, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(text, &glyph_count);
	for(size_t i = 0; i < glyph_count; ++i)
	{
		/** Glyph index 0 in FreeType = invalid glyph, everything is offseted */
		hb_codepoint_t glyph_idx = glyph_info[i].codepoint - 1;
	
		float scale = in_font.size / in_font.font->get_em_size();
		
		hb_position_t x_offset = glyph_pos[i].x_offset * scale;
		hb_position_t y_offset = glyph_pos[i].y_offset * 64.f * scale;
	
		if(!in_font.font->has_glyph(glyph_idx))
			continue;

		const auto& glyph = in_font.font->get_glyph(glyph_idx);
		if(std::isspace(glyph.character))
		{
			size.x += 2 * (in_font.font->get_space_advance() * scale);
			continue;
		}

		float w = glyph.atlas_rect.size.x * scale;
		float h = glyph.atlas_rect.size.y * scale;

		/** Offset from the baselne to properly position the glyph */
		float offset_x = (glyph.bounds.l * scale) + x_offset;
		float offset_y = (-(glyph.bounds.b * 2) * scale) + y_offset;
	
		size.x += (w / 2) + (glyph.advance * scale);
		size.y = std::max(size.y, h - offset_y);
	}

	return size;
}

void DrawCommandPrimitiveText::build(const DrawCommand& command)
{
	effect = { gfx::effect_get_by_name("ZEUIDistanceFieldText"), {} };
	bindings = 
	{ 
		Binding(gfx::DescriptorType::Sampler, 1, 0, gfx::LinearSampler::get()), 
		Binding(gfx::DescriptorType::SampledTexture, 1, 1, font.font->get_texture_view()), 
		Binding(gfx::DescriptorType::UniformBuffer, 1, 2, font_data), 
	};

	vertices.reserve(4 * text_size);
	indices.reserve(6 * text_size);

	/** Baseline/cursor position */
	float current_x = command.position.x;
	float current_y = command.position.y + command.size.y;

	uint32_t glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(text, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(text, &glyph_count);
	size_t idx_offset = 0;
	for(size_t i = 0; i < glyph_count; ++i)
	{
		/** Glyph index 0 in FreeType = invalid glyph, everything is offseted */
		hb_codepoint_t glyph_idx = glyph_info[i].codepoint - 1;
		
		float scale = font.size / font.font->get_em_size();
		
		hb_position_t x_offset = glyph_pos[i].x_offset * scale;
		hb_position_t y_offset = glyph_pos[i].y_offset * 64.f * scale;
	
		if(!font.font->has_glyph(glyph_idx))
			continue;

		const auto& glyph = font.font->get_glyph(glyph_idx);
		if(std::isspace(glyph.character))
		{
			current_x += 2 * (font.font->get_space_advance() * scale);
			continue;
		}

		float u0 = glyph.atlas_rect.position.x / font.font->get_width();
		float u1 = (glyph.atlas_rect.position.x + glyph.atlas_rect.size.x) / font.font->get_width();
		float v0 = glyph.atlas_rect.position.y / font.font->get_height();
		float v1 = (glyph.atlas_rect.position.y + glyph.atlas_rect.size.y) / font.font->get_height();

		float w = glyph.atlas_rect.size.x * scale;
		float h = glyph.atlas_rect.size.y * scale;

		/** Offset from the baselne to properly position the glyph */
		float offset_x = (glyph.bounds.l * scale) + x_offset;
		float offset_y = (-(glyph.bounds.b * 2) * scale) + y_offset;
	
		vertices.emplace_back(maths::Vector2f(current_x + offset_x, current_y + offset_y),
			maths::Vector2f(u0, v0),
			color);
		vertices.emplace_back(maths::Vector2f((current_x + w) + offset_x, current_y + offset_y),
			maths::Vector2f(u1, v0),
			color);
		vertices.emplace_back(maths::Vector2f((current_x + w) + offset_x, (current_y - h) + offset_y),
			maths::Vector2f(u1, v1),
			color);
		vertices.emplace_back(maths::Vector2f((current_x) + offset_x, (current_y - h) + offset_y), 
			maths::Vector2f(u0, v1),
			color);

		indices.emplace_back(idx_offset + 0);
		indices.emplace_back(idx_offset + 1);
		indices.emplace_back(idx_offset + 2);
		indices.emplace_back(idx_offset + 2);
		indices.emplace_back(idx_offset + 3);
		indices.emplace_back(idx_offset + 0);

		idx_offset += 4;
		current_x += (w / 2) + (glyph.advance * scale);
	}
}

}