#pragma once

namespace ZE::Editor
{

class CMapEditor;

/**
 * Widget that display a list of all entities
 */
class CEntityList
{
public:
	CEntityList(CMapEditor& InMapEditor);

	void Draw();
private:
	CMapEditor& MapEditor;
};

}