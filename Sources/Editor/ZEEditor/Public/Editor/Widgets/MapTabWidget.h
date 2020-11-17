#pragma once

#include "Editor/MapEditor.h"
#include "Engine/UI/Console.h"
#include "Editor/AssetExplorer.h"

namespace ze { class World; }

namespace ze::editor
{

/**
 * Main map tab widget
 */
class CMapTabWidget
{
public:
	CMapTabWidget(World& in_world);

	void Draw();

	CAssetExplorer& GetAssetExplorer() { return AssetExplorer; }
	CMapEditor& get_map_editor() { return MapEditor; }
private:
	World& world;
	CMapEditor MapEditor;
	CConsoleWidget Console;
	CAssetExplorer AssetExplorer;
};

}