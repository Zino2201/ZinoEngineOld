#include "ZEUI/Containers/Docking.h"

namespace ze::ui
{

DockManager::DockManager()
{
	for(uint8_t i = 0; i < static_cast<uint8_t>(DockAreaPosition::Max); ++i)
	{
		DockAreaPosition pos = static_cast<DockAreaPosition>(i);
		areas.insert({ pos, DockArea(pos) });
	}
}

void DockManager::add(const std::string id, OwnerPtr<Widget> in_widget, const DockAreaPosition in_position)
{
	/** Prepare a dock tab to host the widget */
	std::unique_ptr<DockTab> tab = make_widget_unique<DockTab>();
	tab->set_parent(this);
	tab->construct();

	/** Try add it to the specified dock area */
	DockArea::Tab& area = areas[in_position].tabs.emplace_back();
	area.widget = tab.get();
	tabs.insert({ id, std::move(tab)});
}

void DockManager::compute_desired_size(maths::Vector2f available_size)
{
	/** Traverse each areas to determine the size of each widget */
	for(auto& [dir, area] : areas)
	{
		for(const auto& tab : area.tabs)
		{
			tab.widget->compute_desired_size(tab.size);
		}
	}

	desired_size = available_size;
}

void DockManager::arrange_children()
{
	for(uint8_t i = 0; i < static_cast<uint8_t>(DockAreaPosition::Max); ++i)
	{
		auto& area = areas[static_cast<DockAreaPosition>(i)];
		for(auto& tab : area.tabs)
		{
			tab.widget->set_arranged_rect(WidgetRect({}, {}, tab.size));
			tab.widget->arrange_children();
		}
	}
}

void DockManager::paint(Renderer& renderer, DrawContext& context)
{
	Widget::paint(renderer, context);

	for(const auto& [id, widget] : tabs)
	{
		widget->paint(renderer, context);
	}
}

}