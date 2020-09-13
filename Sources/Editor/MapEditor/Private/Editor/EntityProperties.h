#pragma once

namespace ZE::Editor
{

class CMapEditor;

/**
 * Widget that display a list of all entities
 */
class CEntityProperties
{
public:
	CEntityProperties(CMapEditor& InMapEditor);

	void Draw();
private:
	CMapEditor& MapEditor;
};

}