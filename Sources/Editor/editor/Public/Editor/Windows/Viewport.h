#pragma once

#include "Window.h"

namespace ze::editor
{

class Viewport : public Window
{
public:
	Viewport();
protected:
	void draw() override;
};

}