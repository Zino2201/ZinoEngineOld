#pragma once

#include "ZEUI/Widget.h"
#include "ZEUI/Item.h"

namespace ze::ui
{

/**
 * A widget displaying text
 */
class ZEUI_API Text final : public Widget
{	
public:
	Text(const std::string_view& text = "")
		: text_(text) {}

	Text* text(const std::string& in_text) { text_ = in_text; return this; }

	void construct() override;
	void paint(Renderer& renderer, DrawContext& context) override;

	void compute_desired_size(const maths::Vector2f& available_size) override;
private:
	std::string text_;
};
	
}