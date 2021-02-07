#pragma once

namespace ze::ui
{

struct Padding
{
	float top;
	float left;
	float right;
	float bottom;

	Padding(const float in_top = 0.f, const float in_left = 0.f,
		const float in_right = 0.f, const float in_bottom = 0.f) :
		top(in_top), left(in_left), right(in_right), bottom(in_bottom) {}
};

}