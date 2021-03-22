#pragma once

#include "Widget.h"
#include "Item.h"

namespace ze::ui
{

/**
 * A widget composed of a single child item
 * This is the recommanded way to create an widget unless you want to create a new custom control 
 *	or container
 */
class ZEUI_API CompositeWidget : public Widget
{
public:
	CompositeWidget() : content_(this) {}

	ZE_FORCEINLINE SimpleItem& content() { return content_; }

	void compute_desired_size(maths::Vector2f in_available_size) override;
	void arrange_children() override;
	void paint(Renderer& renderer, DrawContext& context) override;

	SimpleItem* item() { return &content_; }
protected:
	SimpleItem content_;
};

}