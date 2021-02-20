#include "SDFFontGen.h"
#include <ft2build.h>
#include <numeric>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "Module/Module.h"
#include "Maths/Vector.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace ze::editor
{

FT_Library library;

class SDFFontGenModule : public module::Module
{
public:
	SDFFontGenModule()
	{
		FT_Error a = FT_Init_FreeType(&library);
	}

	~SDFFontGenModule()
	{
		FT_Done_FreeType(library);
	}
};

ZE_DEFINE_MODULE(SDFFontGenModule, SDFFontGen);
	
struct Point
{
	int dx;
	int dy;

	int dist_sqrt() const { return dx * dx + dy * dy; }
};

void compare_point(uint32_t tex_width, uint32_t tex_height, 
	std::vector<Point>& points, size_t x, size_t y, Point& point, int32_t offset_x, int32_t offset_y)
{
	if (x + offset_x < 0 || y + offset_y < 0 
		|| x + offset_x >= tex_width || y + offset_y >= tex_height)
	{
		Point other = { 9999, 9999 };
		other.dx += offset_x;
		other.dy += offset_y;

		if (other.dist_sqrt() < point.dist_sqrt())
			point = other;
	}
	else
	{
		Point other = points[(y + offset_y) * tex_width + (x + offset_x)];
		other.dx += offset_x;
		other.dy += offset_y;

		if (other.dist_sqrt() < point.dist_sqrt())
			point = other;
	}
}

void generate_sdf_data(uint32_t tex_width, uint32_t tex_height, std::vector<Point>& points)
{
	/** Pass 1 */
	for(size_t y = 0; y < tex_height; ++y)
	{
		for(size_t x = 0; x < tex_width; ++x)
		{
			Point& point = points[y * tex_width + x];
			compare_point(tex_width, tex_height, points, x, y, point, -1, 0);
			compare_point(tex_width, tex_height, points, x, y, point, 0, -1);
			compare_point(tex_width, tex_height, points, x, y, point, -1, -1);
			compare_point(tex_width, tex_height, points, x, y, point, 1, -1);
		}

		for(size_t x = tex_width - 1; x != -1; --x)
		{
			Point& point = points[y * tex_width + x];
			compare_point(tex_width, tex_height, points, x, y, point, 1, 0);
		}
	}

	/** Pass 2 */
	for(size_t y = tex_height - 1; y != 0; --y)
	{
		for(size_t x = tex_width - 1; x != 0; --x)
		{
			Point& point = points[y * tex_width + x];
			compare_point(tex_width, tex_height, points, x, y, point, 1, 0);
			compare_point(tex_width, tex_height, points, x, y, point, 0, 1);
			compare_point(tex_width, tex_height, points, x, y, point, -1, 1);
			compare_point(tex_width, tex_height, points, x, y, point, 1, 1);
		}

		for(size_t x = 0; x < tex_width; ++x)
		{
			Point& point = points[y * tex_width + x];
			compare_point(tex_width, tex_height, points, x, y, point, -1, 0);
		}
	}
}

std::vector<uint8_t> generate_sdf_texture_from_font(const std::vector<uint8_t>& in_font_data,
	const uint32_t in_base_font_size,
	const uint32_t spread)
{
	/**
	 * Generate signed distance field data from a font
	 * First we render a bitmap texutre atlas using FreeType
	 * then we use the 8-points Signed Sequential Euclidean Distance Transform algorithm to transform the bitmap
	 * into a signed distance field texture
	 * http://www.codersnotes.com/notes/signed-distance-fields/
	 */

	/** Render glyphs to a bitmap */

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

	FT_Set_Char_Size(face, 0, 16 * in_base_font_size, 0, 0);

	size_t num_glyphs = 127 - 33;
	size_t tex_width = 256;
	size_t tex_height = 256;
	std::vector<uint8_t> bitmap_data;
	bitmap_data.resize(tex_width * tex_height);
	uint32_t cur_x = spread;
	uint32_t cur_y = spread;

	for(char ch = 33; ch < 127; ++ch)
	{
		/** Load the current char glyph and render it */
		FT_Load_Char(face, ch, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL);

		if(cur_x + face->glyph->bitmap.width + spread >= tex_width)
		{
			cur_x = spread;
			cur_y += (face->size->metrics.height >> 6) + 1 + spread;
		}

		/** Copy it to bitmap_data */
		uint8_t* src = face->glyph->bitmap.buffer;
		for(size_t y = 0; y < face->glyph->bitmap.rows; ++y)
		{
			for(size_t x = 0; x < face->glyph->bitmap.width; ++x)
			{
				uint32_t abs_x = cur_x + x;
				uint32_t abs_y = cur_y + y;
				bitmap_data[abs_y * tex_width + abs_x] = face->glyph->bitmap.buffer[y * face->glyph->bitmap.pitch + x];
			}

			src += face->glyph->bitmap.pitch;
		}

		cur_x += (face->glyph->bitmap.width + 1) + spread;
	}

	/** Free the FreeType face as we no longer need it */
	FT_Done_Face(face);

	/** Now generate a signed distance field texture from the bitmap */
	std::vector<uint8_t> sdf_data;
	std::vector<Point> points_0;
	std::vector<Point> points_1;
	sdf_data.resize(tex_width * tex_height);
	points_0.resize(tex_width * tex_height);
	points_1.resize(tex_width * tex_height);

	/** Initialize points */
	for(size_t y = 0; y < tex_height; ++y)
	{
		for(size_t x = 0; x < tex_width; ++x)
		{
			if(bitmap_data[y * tex_width + x] > 0)
			{
				points_0[y * tex_width + x] = { 0, 0 };
				points_1[y * tex_width + x] = { 9999, 9999 };
			}
			else
			{
				points_0[y * tex_width + x] = { 9999, 9999 };
				points_1[y * tex_width + x] = { 0, 0 };
			}
		}
	}

	/** Generate SDF data into the 2 arrays */
	generate_sdf_data(tex_width, tex_height, points_0);
	generate_sdf_data(tex_width, tex_height, points_1);

	/** Construct the final SDF data and store it */
	for(size_t y = 0; y < tex_height; ++y)
	{
		for(size_t x = 0; x < tex_width; ++x)
		{
			int dist1 = (int)(sqrt((double)points_0[y * tex_width + x].dist_sqrt()));
			int dist2 = (int)(sqrt((double)points_1[y * tex_width + x].dist_sqrt()));
			int dist = dist1 - dist2;
			
			dist *= 3;
			dist += 128;
				
			if (dist < 0) 
				dist = 0;
			if (dist > 255) 
				dist = 255;

			sdf_data[y * tex_width + x] = dist;
		}
	}

	return sdf_data;
}

}