#include "ZEUI/Containers/Docking.h"
#include "ZEUI/Primitives/Box.h"

namespace ze::ui
{

void DockTab::construct()
{
	content().size_mode(SizeMode::Fill);
	content()
	[
		make_widget<Box>()
		->brush(Brush::make_color({ 0, 0, 1, 1}))
	];
}

}