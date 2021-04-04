#include "Editor/Windows/MapEditor.h"
#include "Editor/Windows/AssetExplorer.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

MapEditor::MapEditor()
	: Window("Untitled Map")
{
	add(new AssetExplorer);
}

void MapEditor::draw()
{
	ImGui::TextUnformatted("Map Editor");
}

}