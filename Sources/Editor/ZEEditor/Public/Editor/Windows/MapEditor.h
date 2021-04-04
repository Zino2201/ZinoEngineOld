#pragma once

#include "EngineCore.h"
#include "Window.h"

namespace ze::editor
{

/**
 * The main window that enable map editing
 * w/ asset explorer, entity properties etc
 */
class MapEditor : public Window
{
public:
	MapEditor();
protected:
	void draw() override;
};

}