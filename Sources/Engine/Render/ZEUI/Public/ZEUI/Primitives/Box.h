#pragma once

#include "ZEUI/CompositeWidget.h"
#include "ZEUI/Brush.h"

namespace ze::ui
{


/**
 * A simple box widget that can have a background color or image
 */
class Box : public CompositeWidget
{
public:
	Box() {}

	Box* brush(const Brush& in_brush) { brush_ = in_brush; return this; }

	void paint(Renderer& renderer, DrawContext& context) override;
private:
	Brush brush_;
};

}