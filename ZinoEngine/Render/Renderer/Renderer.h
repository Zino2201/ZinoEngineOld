#pragma once

#include "Render/RenderCore.h"

class CRenderCommandList;

/**
 * Renderer base interface
 * Used to render a scene
 */
class IRenderer
{
public:
	/**
	 * Render the scene
	 * Called by the render thread
	 */
	virtual void Render(CRenderCommandList* InCommandList) = 0;
};