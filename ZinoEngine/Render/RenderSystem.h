#pragma once

#include "RenderCore.h"

class IRenderCommandContext;

/**
 * Render system interface
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	virtual void Initialize() = 0;
	virtual IRenderCommandContext* CreateCommandContext() = 0;
};