#pragma once

#include "Window.h"

namespace ze::editor
{

class Tools : public Window
{
public:
	Tools();
protected:
	void draw() override;
};

}