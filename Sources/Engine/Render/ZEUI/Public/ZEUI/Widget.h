#pragma once

#include "EngineCore.h"
#include "Maths/Vector.h"

namespace ze::ui
{

class Widget;
class DrawContext;
class Renderer;

struct WidgetRect
{
	maths::Vector2f absolute_position;
	maths::Vector2f position;
	maths::Vector2f size;

	WidgetRect() = default;
	WidgetRect(const maths::Vector2f& in_position,
		const maths::Vector2f& in_absolute_position,
		const maths::Vector2f& in_size) : position(in_position), absolute_position(in_absolute_position),
		size(in_size) {}
};

/**
 * Base class of all UI widgets
 */
class ZEUI_API Widget
{
public:
	Widget();
	virtual ~Widget() = default;

	void set_parent(Widget* in_parent) { parent = in_parent; }

	/**
	 * Compute the desired widget size
	 */
	virtual void compute_desired_size(const maths::Vector2f& available_size) = 0;

	/**
	 * Arrange children widgets
	 */
	virtual void arrange_children() {}

	virtual void construct();
	virtual void paint(Renderer& renderer, DrawContext& context);

	void set_arranged_rect(const WidgetRect& in_arranged_rect) { arranged_rect = in_arranged_rect; }

	ZE_FORCEINLINE const maths::Vector2f& get_desired_size() const { return desired_size; }
	ZE_FORCEINLINE const WidgetRect& get_arranged_rect() const { return arranged_rect; }
protected:
	Widget* parent;
	maths::Vector2f desired_size;
	WidgetRect arranged_rect;
	bool size_changed;
	bool children_arrangement_changed;
};
	
	
}