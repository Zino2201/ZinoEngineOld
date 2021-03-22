#pragma once

#include "EngineCore.h"
#include "Widget.h"

namespace ze::ui
{

enum class HorizontalAlignMode
{
	Left,
	Center,
	Right,
};

enum class VerticalAlignMode
{
	Top,
	Center,
	Bottom
};

enum class SizeMode
{
	Auto,

	/** Fill parent */
	Fill
};

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

	template<typename OtherItemType>
		requires std::derived_from<OtherItemType, Item<OtherItemType>>
	ItemType& operator[](const OtherItemType& item)
	{
		this_.add(item.owner);
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
public:
	Widget* owner;
	std::unique_ptr<Widget> widget;
protected:
	ItemType& this_;
};

/**
 * A simple item with horizontal/vertical alignement
 */
class ZEUI_API SimpleItem : public Item<SimpleItem>
{
public:
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

	SimpleItem* horizontal_align(HorizontalAlignMode in_align_mode) { horizontal_align_mode = in_align_mode; return this; }
	SimpleItem* size_mode(SizeMode in_size_mode) { size_mode_ = in_size_mode; return this; }

	ZE_FORCEINLINE HorizontalAlignMode get_horizontal_align_mode() const { return horizontal_align_mode; }
	ZE_FORCEINLINE SizeMode get_size_mode() const { return size_mode_; }
private:
	HorizontalAlignMode horizontal_align_mode;
	SizeMode size_mode_;
};

template<typename T, typename... Args>
ZE_FORCEINLINE OwnerPtr<T> make_widget(Args&&... in_args)
{
	return new T(std::forward<Args>(in_args)...);
}

template<typename T, typename... Args>
ZE_FORCEINLINE OwnerPtr<T> make_widget(T*& in_ptr_to_assign, Args&&... in_args)
{
	in_ptr_to_assign = new T(std::forward<Args>(in_args)...);
	return in_ptr_to_assign;
}

template<typename T, typename... Args>
ZE_FORCEINLINE std::unique_ptr<T> make_widget_unique(Args&&... in_args)
{
	return std::make_unique<T>(std::forward<Args>(in_args)...);
}

}