#pragma once

#include "Engine/ECS/ECS.h"

namespace ze::editor
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
	void set_entity(Entity in_selected_entity) { selected_entity = in_selected_entity; }
private:
	CMapEditor& MapEditor;
	Entity selected_entity;
};

}