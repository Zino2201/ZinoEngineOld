#include "ZEUI/Primitives/DockableTab.h"
#include "ImGui/ImGui.h"

namespace ze::ui
{

void DockableTab::construct()
{
	if(title_.empty())
	{
		title_ = "Unnamed Dockable Tab " + std::to_string(reinterpret_cast<uintptr_t>(this));
	}

	CompositeWidget::construct();
}

}