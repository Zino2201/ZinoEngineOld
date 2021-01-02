#pragma once

#include "EngineCore.h"
#include "Widget.h"

namespace ze::ui
{

/*
 * A item contained in a Container widget
 * This wraps a widget and allow additional per-child information
 */
template<typename ItemType>
class ZEUI_API Item
{
public:
	Item() : owner(nullptr), this_(*static_cast<ItemType*>(this)) {}
	Item(Widget* in_owner) : owner(in_owner), this_(*static_cast<ItemType*>(this)) {}
	virtual ~Item() = default;

	ZE_FORCEINLINE Widget* get_owner() { return owner; }
	ZE_FORCEINLINE bool is_valid() { return widget.get(); }

	ItemType& operator[](OwnerPtr<Widget> item)
	{
		this_.add(item);
		ZE_CHECK(widget);
		return this_;
	}

	Widget& get() 
	{ 
		ZE_CHECK(widget); 
		return *widget.get(); 
	}

	ItemType& content() { return this_; }

	void set_owner(Widget* in_owner) { owner = in_owner; }
protected:
	Widget* owner;
	std::unique_ptr<Widget> widget;
	ItemType& this_;
};

struct ZEUI_API SimpleItem : public Item<SimpleItem>
{
	SimpleItem() {}
	SimpleItem(Widget* in_owner) : Item(in_owner) {}

	void add(OwnerPtr<Widget> in_widget)
	{
		ZE_CHECK(in_widget);
		ZE_CHECK(owner);
		widget = std::unique_ptr<Widget>(in_widget);
		widget->set_parent(owner);
		widget->construct();
	}
};

template<typename T, typename... Args>
ZE_FORCEINLINE OwnerPtr<T> make_widget(Args&&... in_args)
{
	return new T(std::forward<Args>(in_args)...);
}

template<typename T, typename... Args>
ZE_FORCEINLINE std::unique_ptr<T> make_widget_unique(Args&&... in_args)
{
	return std::make_unique<T>(std::forward<Args>(in_args)...);
}

}