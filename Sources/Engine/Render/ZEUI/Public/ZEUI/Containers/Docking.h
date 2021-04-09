#pragma once

#include "ZEUI/Container.h"
#include "ZEUI/CompositeWidget.h"
#include <robin_hood.h>

namespace ze::ui
{

class DockTab;

enum class DockAreaPosition : uint8_t
{
	Up,
	Down,
	Left,
	Right,
	Center,
	Tabbed,
	Max
};

/**
 * Manage multiple DockTabs across DockAreas
 */
class DockManager : public Container
{
	/** A single dock area */
	struct DockArea
	{
		struct Tab
		{
			DockTab* widget;
			maths::Vector2f position;
			maths::Vector2f size;
			bool has_been_added;

			Tab() : widget(nullptr), has_been_added(true) {}
		};

		DockAreaPosition position;
		std::vector<Tab> tabs;

		DockArea() {}
		DockArea(const DockAreaPosition in_position) : position(in_position) {}

		void invalidate_size();
	};

	struct Node
	{

	};
public:
	DockManager();
	void add(const std::string id, OwnerPtr<Widget> in_widget, const DockAreaPosition in_position);
	void compute_desired_size(maths::Vector2f available_size) override;
	void arrange_children() override;
	void paint(Renderer& renderer, DrawContext& context) override;
private:
	robin_hood::unordered_map<std::string, std::unique_ptr<DockTab>> tabs;
	robin_hood::unordered_map<DockAreaPosition, DockArea> areas;
};

/**
 * A dockable tab that can float around in a FloatingWidget or be docked or tabbed
 * Managed by a DockManager
 */
class DockTab : public CompositeWidget
{
public:
	void construct() override;
};

}