#pragma once

#include "ImGui/ImGui.h"

namespace ZE::Editor
{

class CEntityList;
class CEntityProperties;

/**
 * A map editor
 */
class MAPEDITOR_API CMapEditor
{
public:
	CMapEditor();
	~CMapEditor();

	void Draw(const ImGuiID& InDockspaceID);
private:
	std::unique_ptr<CEntityList> EntityList;
	std::unique_ptr<CEntityProperties> EntityProperties;
};

}