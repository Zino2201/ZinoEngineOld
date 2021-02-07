#pragma once

#include "Widget.h"
#include "Item.h"

namespace ze::ui
{

struct ZEUI_API ContainerItem : public Item<ContainerItem>
{
	ContainerItem() {}
	ContainerItem(Widget* in_owner) : Item(in_owner) {}

	void add(OwnerPtr<Widget> in_widget)
	{
		widget = std::unique_ptr<Widget>(in_widget);
	}
};

/**
 * Containers are widget that store multiples widgets wrapping them in Items
 */
class ZEUI_API Container : public Widget
{
public:
	Container() {}

	Container(const Container&) = delete;
	void operator=(const Container&) = delete;

	void add(ContainerItem* in_item);
	void compute_desired_size(const maths::Vector2f& available_size) override;
	void paint(Renderer& renderer, DrawContext& context) override;
protected:
	std::vector<std::unique_ptr<ContainerItem>> items;
};

/**
 * Heap allocate a new container item
 * Must be freed or used with a smart pointer !
 */
template<typename T, typename... Args>
	requires std::derived_from<typename T::Item, ContainerItem>
ZE_FORCEINLINE typename T::Item* make_item(Args&&... in_args)
{
	return new typename T::Item(std::forward<Args>(in_args)...);
}

}

template<typename T>
	requires std::derived_from<T, ze::ui::Container>
T* operator+(T* in_widget, ze::ui::ContainerItem& in_item)
{
	in_widget->add(&in_item);
	return in_widget;
}