#pragma once

#include "Render/RenderCore.h"

class CRenderCommandList;
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
	virtual void Render(CRenderCommandList* InCommandList,
		CScene* InScene) = 0;
};