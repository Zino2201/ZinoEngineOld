#pragma once

#include "Vector.h"

namespace ze::maths
{

/*
 * A 2D rectangle
 */
struct Rect2D
{
	maths::Vector2f position;
	maths::Vector2f size;

	Rect2D() {}

	Rect2D(const float x, const float y,
		const float w, const float h) : position(x, y), size(w, h) {}
	Rect2D(const maths::Vector2f& in_position,
		const maths::Vector2f& in_size) : position(in_position),
		size(in_size) {}
};

}