#include "Editor/Windows/MapEditor.h"
#include "Editor/Windows/AssetExplorer.h"
#include "Editor/Windows/Viewport.h"
#include "Editor/Windows/EntityList.h"
#include "Editor/Windows/EntityProperties.h"
#include "Editor/Windows/Tools.h"
#include "ImGui/ImGui.h"

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