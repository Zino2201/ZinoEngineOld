#include "ZEUI/Render/DrawCommandText.h"
#include "Gfx/Effect/EffectDatabase.h"
#include "Gfx/Gfx.h"
#include "ImGui/ImGui.h"
#include <hb.h>

namespace ze::ui
{

maths::Vector2f DrawCommandPrimitiveText::measure(TextAlignMode align_mode,
	hb_buffer_t* text, const FontInfo& font)
{
	maths::Vector2f size;
		
	maths::Vector2f cursor_pos;
	uint32_t glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(text, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(text, &glyph_count);
	if(align_mode == TextAlignMode::Right)
	{
		for(size_t i = glyph_count; i-- > 0;)
		{
			/** Glyph index 0 in FreeType = invalid glyph, everything is offseted */
			hb_codepoint_t glyph_idx = glyph_info[i].codepoint - 1;
		
			float scale = font.size / font.font->get_em_size();
		
			hb_position_t x_offset = glyph_pos[i].x_offset * scale;
			hb_position_t y_offset = glyph_pos[i].y_offset * 64.f * scale;
	
			if(!font.font->has_glyph(glyph_idx))
				continue;

			const auto& glyph = font.font->get_glyph(glyph_idx);
		
			float w = glyph.atlas_rect.size.x * scale;
			float h = glyph.atlas_rect.size.y * scale;

			/** Support space */
			if(std::isspace(glyph.character))
			{
				if(align_mode == TextAlignMode::Right)
					size.x -= 2 * (font.font->get_space_advance() * scale);
				else
					size.x += 2 * (font.font->get_space_advance() * scale);

				continue;
			}

			/** Offset from the baselne to properly position the glyph */
			float offset_x = (glyph.bounds.l * scale) + x_offset;
			float offset_y = (-(glyph.bounds.b * 2) * scale) + y_offset;

			size.x += (w / 2) + (glyph.advance * scale);
			size.y = std::max(size.y, h - offset_y);
		}
	}
	else
	{
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
		
			float w = glyph.atlas_rect.size.x * scale;
			float h = glyph.atlas_rect.size.y * scale;

			/** Support space */
			if(std::isspace(glyph.character))
			{
				if(align_mode == TextAlignMode::Right)
					size.x -= 2 * (font.font->get_space_advance() * scale);
				else
					size.x += 2 * (font.font->get_space_advance() * scale);

				continue;
			}

			/** Offset from the baselne to properly position the glyph */
			float offset_x = (glyph.bounds.l * scale) + x_offset;
			float offset_y = (-(glyph.bounds.b * 2) * scale) + y_offset;

			size.x += (w / 2) + (glyph.advance * scale);
			size.y = std::max(size.y, h - offset_y);
		}
	}

	return size;
}

void DrawCommandPrimitiveText::print(hb_glyph_info_t& glyph_info, hb_glyph_position_t& glyph_pos)
{
	/** Glyph index 0 in FreeType = invalid glyph, everything is offseted */
	hb_codepoint_t glyph_idx = glyph_info.codepoint - 1;
		
	float scale = font.size / font.font->get_em_size();
		
	hb_position_t x_offset = glyph_pos.x_offset * scale;
	hb_position_t y_offset = glyph_pos.y_offset * 64.f * scale;
	
	if(!font.font->has_glyph(glyph_idx))
		return;

	const auto& glyph = font.font->get_glyph(glyph_idx);
		
	float w = glyph.atlas_rect.size.x * scale;
	float h = glyph.atlas_rect.size.y * scale;
		
	/** Support space */
	if(std::isspace(glyph.character))
	{
		if(align_mode == TextAlignMode::Right)
			cursor_pos.x -= 2 * (font.font->get_space_advance() * scale);
		else
			cursor_pos.x += 2 * (font.font->get_space_advance() * scale);

		return;
	}
	// TODO: Support TAB	
	else if(align_mode == TextAlignMode::Right)
	{
		cursor_pos.x -= w;
	}

	float u0 = glyph.atlas_rect.position.x / font.font->get_width();
	float u1 = (glyph.atlas_rect.position.x + glyph.atlas_rect.size.x) / font.font->get_width();
	float v0 = glyph.atlas_rect.position.y / font.font->get_height();
	float v1 = (glyph.atlas_rect.position.y + glyph.atlas_rect.size.y) / font.font->get_height();

	/** Offset from the baselne to properly position the glyph */
	float offset_x = (glyph.bounds.l * scale) + x_offset;
	float offset_y = (-(glyph.bounds.b * 2) * scale) + y_offset;

	vertices.emplace_back(maths::Vector2f(cursor_pos.x + offset_x, cursor_pos.y + offset_y),
		maths::Vector2f(u0, v0),
		color);
	vertices.emplace_back(maths::Vector2f((cursor_pos.x + w) + offset_x, cursor_pos.y + offset_y),
		maths::Vector2f(u1, v0),
		color);
	vertices.emplace_back(maths::Vector2f((cursor_pos.x + w) + offset_x, (cursor_pos.y - h) + offset_y),
		maths::Vector2f(u1, v1),
		color);
	vertices.emplace_back(maths::Vector2f((cursor_pos.x) + offset_x, (cursor_pos.y - h) + offset_y), 
		maths::Vector2f(u0, v1),
		color);

	indices.emplace_back(idx_offset + 0);
	indices.emplace_back(idx_offset + 1);
	indices.emplace_back(idx_offset + 2);
	indices.emplace_back(idx_offset + 2);
	indices.emplace_back(idx_offset + 3);
	indices.emplace_back(idx_offset + 0);

	idx_offset += 4;

	/** Advance cursor */
	if(align_mode == TextAlignMode::Right)
	{
		cursor_pos.x -= (w / 2) - (glyph.advance * scale);
	}
	else
	{
		cursor_pos.x += (w / 2) + (glyph.advance * scale);
	}
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
	cursor_pos = maths::Vector2f(command.position.x, command.position.y + command.size.y);
	uint32_t glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(text, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(text, &glyph_count);
	if(align_mode == TextAlignMode::Right)
	{
		cursor_pos.x += command.size.x;

		for(size_t i = glyph_count; i-- > 0;)
		{
			print(glyph_info[i], glyph_pos[i]);	
		}
	}
	else
	{
		for(size_t i = 0; i < glyph_count; ++i)
		{
			print(glyph_info[i], glyph_pos[i]);	
		}
	}
}

}