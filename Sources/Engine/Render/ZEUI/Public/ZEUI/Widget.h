#pragma once

#include "EngineCore.h"
#include "Maths/Vector.h"

namespace ze::ui
{

/**
 * Base class of all UI widgets
 */
class ZEUI_API Widget
{
public:
	virtual ~Widget() = default;

	void set_parent(Widget* in_parent) { parent = in_parent; }

	virtual void construct();
	virtual void paint();

	ZE_FORCEINLINE const maths::Vector2f& get_size() const { return size; }
protected:
	Widget* parent;
	maths::Vector2f size;
};
	
	
}