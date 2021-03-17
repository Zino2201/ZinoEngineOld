#include "SDFFontGen.h"
#include <ft2build.h>
#include <numeric>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "Module/Module.h"
#include "Maths/Vector.h"
#include "msdfgen.h"
#include "msdfgen-ext.h"
#include "msdf-atlas-gen.h"
#include "AtlasGenerator.h"
#include "TightAtlasPacker.h"

namespace ze::editor
{

FT_Library library;

using namespace msdfgen;

class SDFFontGenModule : public module::Module
{
public:
	SDFFontGenModule()
	{
		FT_Init_FreeType(&library);
	}

	~SDFFontGenModule()
	{
		FT_Done_FreeType(library);
	}
};

ZE_DEFINE_MODULE(SDFFontGenModule, SDFFontGen);


using namespace msdf_atlas;

MSDFFontData generate_msdf_texture_from_font(const std::vector<uint8_t>& in_font_data,
	const uint32_t in_base_font_size)
{
	MSDFFontData ret;

	FT_Face face;
	FT_Error error = FT_New_Memory_Face(library, 
		in_font_data.data(),
		in_font_data.size(),
		0,
		&face);
	if(error != FT_Err_Ok)
	{
		ze::logger::error("Failed to generate SDF font data: can't create font face ({})", error);
		return {};
	}
	
	FontHandle* font = adoptFreetypeFont(face);

	/** Enumerate glyphs we want to generate a distance field */
	size_t num_glyphs = 127 - 33;
	std::vector<GlyphGeometry> glyphs;
	glyphs.reserve(num_glyphs);

	/** ASCII codepoint */
	for(char ch = 0; ch < 127; ++ch)
	{
		glyphs.emplace_back();
		glyphs.back().load(font, ch);
		glyphs.back().edgeColoring(3.0, 2201);
	}

	/** Pack the glyphs into a atlas */
	FontMetrics font_metrics;
	getFontMetrics(font_metrics, font);
	ret.em_size = font_metrics.emSize;

	TightAtlasPacker packer;
	packer.setScale(font_metrics.emSize / in_base_font_size);
	packer.setPixelRange(2);
	packer.setMiterLimit(1);
	packer.setPadding(2);
	packer.setDimensionsConstraint(TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_SQUARE);
	packer.pack(glyphs.data(), glyphs.size());

	/** Determine the width/height of the texture */
	int tex_width = 0, tex_height = 0;
	packer.getDimensions(tex_width, tex_height);
	ret.width = tex_width;
	ret.height = tex_height;

	ze::logger::info("Generating {}x{} multi-channel signed distance field font texture", tex_width, tex_height);

	ImmediateAtlasGenerator<float, 4, mtsdfGenerator, BitmapAtlasStorage<float, 4>> gen(tex_width, tex_height);
	gen.generate(glyphs.data(), glyphs.size());
	BitmapConstRef<float, 4> bitmap = (msdfgen::BitmapConstRef<float, 4>) gen.atlasStorage();
	ret.raw_data.resize(bitmap.width * bitmap.height * 4);

	std::vector<uint8_t>::iterator it = ret.raw_data.begin();
	for(int y = 0; y < bitmap.height; ++y)
	{
		for(int x = 0; x < bitmap.width; ++x)
		{
			*it++ = static_cast<uint8_t>(std::clamp(256.f * bitmap(x, y)[0], 0.f, 255.f));
			*it++ = static_cast<uint8_t>(std::clamp(256.f * bitmap(x, y)[1], 0.f, 255.f));
			*it++ = static_cast<uint8_t>(std::clamp(256.f * bitmap(x, y)[2], 0.f, 255.f));
			*it++ = static_cast<uint8_t>(std::clamp(256.f * bitmap(x, y)[3], 0.f, 255.f));
		}
	}

	getFontWhitespaceWidth(ret.space_advance, ret.tab_advance, font);
	destroyFont(font);
		
	ret.glyphs.reserve(glyphs.size());
	for(const auto& glyph : glyphs)
	{
		if(glyph.getIndex() == 0)
			continue;

		int x, y, w, h;
		glyph.getBoxRect(x, y, w, h);
		
		double l, b, r, t;
		glyph.getQuadPlaneBounds(l, b, r, t);

		ret.glyphs.emplace_back(glyph.getCodepoint(),
			glyph.getAdvance(),
			MSDFGlyph::Bounds { l, b, r, t },
			maths::Rect2D(x, y, w, h));

		ret.glyphs.back().glyph_idx = glyph.getIndex();
	}
	
	/** Sort the glyphs by glyph IDX */
	std::sort(ret.glyphs.begin(), ret.glyphs.end(),
		[](const MSDFGlyph& left, const MSDFGlyph& right)
		{
			return left.glyph_idx < right.glyph_idx;
		});
	/** Free the FreeType face as we no longer need it */
	FT_Done_Face(face);

	return ret;
}

}