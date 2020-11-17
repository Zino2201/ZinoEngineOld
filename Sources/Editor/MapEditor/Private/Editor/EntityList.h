#pragma once

#include "Delegates/Delegate.h"
#include "Engine/ECS/ECS.h"

namespace ze { class World; }

namespace ze::editor
{

class CMapEditor;

/**
 * Widget that display a list of all entities
 */
class CEntityList
{
public:
	CEntityList(World& in_world, CMapEditor& InMapEditor);

	void Draw();
private:
	World& world;
	CMapEditor& MapEditor;
	Entity selected_entity;
public:
	DelegateNoRet<Entity> on_entity_selected;
};

}