#pragma once

#include "Engine/ECS/ECS.h"

namespace ze::editor
{

class CMapEditor;

/**
 * Widget that display a list of all entities
 */
class EntityProfiler
{
public:
	EntityProfiler(CMapEditor& InMapEditor);

	void draw();
private:
	CMapEditor& map_editor;
};

}