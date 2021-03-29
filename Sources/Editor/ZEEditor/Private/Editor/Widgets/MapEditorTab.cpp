#include "Editor/Widgets/MapEditorTab.h"
#include "Editor/Widgets/MapEditor/EntityList.h"
#include "Editor/Widgets/MapEditor/EntityProperties.h"
#include "ZEUI/Primitives/Text.h"
#include "ZEUI/Primitives/DockSpace.h"

namespace ze::editor
{

void MapEditorTab::construct()
{
	using namespace ui;

	title("Map Editor");

	content() 
	[
		make_widget<DockSpace>("MapEditor_DockSpace")
		+ make_item<DockSpace>()
		->content()
		[
			make_widget<ZEUIEntityList>()
		]
		+ make_item<DockSpace>()
		->content()
		[
			make_widget<ZEUIEntityProperties>()
		]
	];

	DockableTab::construct();
}

}