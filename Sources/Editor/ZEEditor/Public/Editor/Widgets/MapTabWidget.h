#pragma once

#include "Editor/MapEditor.h"
#include "Engine/UI/Console.h"
#include "Editor/AssetExplorer.h"

namespace ZE::Editor
{

/**
 * Main map tab widget
 */
class CMapTabWidget
{
public:
	void Draw();

	CAssetExplorer& GetAssetExplorer() { return AssetExplorer; }
private:
	CMapEditor MapEditor;
	CConsoleWidget Console;
	CAssetExplorer AssetExplorer;
};

}