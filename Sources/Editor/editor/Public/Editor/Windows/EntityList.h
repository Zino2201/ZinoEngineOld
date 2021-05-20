#pragma once

#include "Window.h"

namespace ze::editor
{

class EntityList : public Window
{
public:
	EntityList();
protected:
	void draw() override;
};

}