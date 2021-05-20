#include "editor/windows/MapEditor.h"
#include "editor/windows/AssetExplorer.h"
#include "editor/windows/Viewport.h"
#include "editor/windows/EntityList.h"
#include "editor/windows/EntityProperties.h"
#include "editor/windows/Tools.h"
#include "imgui/ImGui.h"

namespace ze::editor
{

MapEditor::MapEditor()
	: Window("Untitled Map", WindowFlagBits::HasExplicitDockSpace)
{
	add(new AssetExplorer);
	add(new Viewport);
	add(new EntityList);
	add(new EntityProperties);
	add(new Tools);
}

void MapEditor::draw()
{
	
}

}