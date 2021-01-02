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

	void paint() override;
private:
	SimpleItem content_;
};

}