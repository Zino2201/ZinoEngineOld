#pragma once

#include "Render/RenderCore.h"

class IRenderCommandContext;
class CScene;

/**
 * Scene renderer base interface
 * Used to render a CScene
 */
class ISceneRenderer
{
public:
	/**
	 * Render a scene
	 * Should be called by the render thread
	 */
	virtual void Render(IRenderCommandContext* InCommandContext,
		CScene* InScene) = 0;
};