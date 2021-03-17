#pragma once

namespace ze::maths
{

/**
 * A linear normalized RGBA 32-bit color
 */
struct Color
{
	union
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};

		std::array<float, 4> comps;
	};

	Color() : r(0), g(0), b(0), a(0) {}
	Color(const float in_r, const float in_g, const float in_b, const float in_a = 1.f) : r(in_r), g(in_g), b(in_b), a(in_a) {}
};

}