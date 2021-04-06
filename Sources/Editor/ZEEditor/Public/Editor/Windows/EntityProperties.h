#pragma once

#include "Window.h"

namespace ze::editor
{

class EntityProperties : public Window
{
public:
	EntityProperties();
protected:
	void draw() override;
};

}