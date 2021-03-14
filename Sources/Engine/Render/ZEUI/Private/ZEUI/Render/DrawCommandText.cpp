#include "ZEUI/Render/DrawCommandText.h"
#include "Gfx/Effect/EffectDatabase.h"
#include "Gfx/Gfx.h"
#include "ImGui/ImGui.h"
#include <hb.h>

namespace ze::ui
{

void DrawCommandPrimitiveText::build(const DrawCommand& command)
{
	effect = { gfx::effect_get_by_name("ZEUIDistanceFieldText"), {} };
	bindings = 
	{ 
		Binding(gfx::DescriptorType::Sampler, 1, 0, gfx::NearestSampler::get()), 
		Binding(gfx::DescriptorType::SampledTexture, 1, 1, font.font->get_texture_view()), 
		Binding(gfx::DescriptorType::UniformBuffer, 1, 2, font_data), 
	};

	vertices.reserve(4 * text_size);
	indices.reserve(6 * text_size);

	/** Baseline/cursor position */
	float current_x = command.position.x;
	float current_y = command.position.y;

	uint32_t glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(text, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(text, &glyph_count);
	size_t idx_offset = 0;
	for(size_t i = 0; i < glyph_count; ++i)
	{
		hb_codepoint_t ch = glyph_info[i].codepoint;
		hb_position_t x_offset = glyph_pos[i].x_offset;
		hb_position_t y_offset = glyph_pos[i].y_offset;
		hb_position_t x_advance = glyph_pos[i].x_advance;
		hb_position_t y_advance = glyph_pos[i].y_advance;

		if(ch == '\0')
			continue;

		if(std::isspace(ch))
		{
			current_x += 2 * font.font->get_space_advance();
			continue;
		}

		if(!font.font->has_glyph(ch))
		{
			if(!font.font->has_glyph('?'))
				return;

			ch = '?';
		}

		const auto& glyph = font.font->get_glyph(ch);

		float u0 = glyph.atlas_rect.position.x / font.font->get_width();
		float u1 = (glyph.atlas_rect.position.x + glyph.atlas_rect.size.x) / font.font->get_width();
		float v0 = glyph.atlas_rect.position.y / font.font->get_height();
		float v1 = (glyph.atlas_rect.position.y + glyph.atlas_rect.size.y) / font.font->get_height();

		float w = glyph.atlas_rect.size.x;
		float h = glyph.atlas_rect.size.y;

		/** Offset from the baselne to properly position the glyph */
		float offset_x = glyph.bounds.l;
		float offset_y = -(glyph.bounds.b * 2) + command.size.y;
	
		vertices.emplace_back(maths::Vector2f(current_x + offset_x, current_y + offset_y),
			maths::Vector3f(1, 0, 0),
			maths::Vector2f(u0, v0));
		vertices.emplace_back(maths::Vector2f((current_x + w) + offset_x, current_y + offset_y), 
			maths::Vector3f(1, 0, 0), 
			maths::Vector2f(u1, v0));
		vertices.emplace_back(maths::Vector2f((current_x + w) + offset_x, (current_y - h) + offset_y), 
			maths::Vector3f(1, 0, 0), 
			maths::Vector2f(u1, v1));
		vertices.emplace_back(maths::Vector2f((current_x) + offset_x, (current_y - h) + offset_y), 
			maths::Vector3f(1, 0, 0),
			maths::Vector2f(u0, v1));

		indices.emplace_back(idx_offset + 0);
		indices.emplace_back(idx_offset + 1);
		indices.emplace_back(idx_offset + 2);
		indices.emplace_back(idx_offset + 2);
		indices.emplace_back(idx_offset + 3);
		indices.emplace_back(idx_offset + 0);

		idx_offset += 4;
		current_x += (w / 2) + glyph.advance;
	}
}

}