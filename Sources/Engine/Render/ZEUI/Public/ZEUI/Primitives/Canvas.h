#pragma once

#include "ZEUI/Container.h"
#include "ZEUI/Item.h"
#include "Maths/Vector.h"

namespace ze::ui
{

struct CanvasAnchor
{
	maths::Vector2f anchor;

	CanvasAnchor() {}
	CanvasAnchor(const maths::Vector2f& in_anchor) : anchor(in_anchor) {}
};

class CanvasItem : public ContainerItem
{
public:
	CanvasItem() = default;
	CanvasItem(Widget* in_owner) : ContainerItem(in_owner) {}
	
	CanvasItem* position(const maths::Vector2f& in_position) { position_ = in_position; return this; }
	CanvasItem* anchor(const CanvasAnchor& in_anchor) { anchor_ = in_anchor; return this; }
	CanvasItem* alignement(const maths::Vector2f& in_alignement) { alignement_ = in_alignement; return this; }

	ZE_FORCEINLINE const maths::Vector2f& get_position() const { return position_; }
	ZE_FORCEINLINE const CanvasAnchor& get_anchor() const { return anchor_; }
	ZE_FORCEINLINE const maths::Vector2f& get_alignement() const { return alignement_; }
private:
	maths::Vector2f position_;
	CanvasAnchor anchor_;
	maths::Vector2f alignement_;
}; 

class ZEUI_API Canvas final : public Container
{
public:
	using Item = CanvasItem;

	Canvas() {}
};
	
}