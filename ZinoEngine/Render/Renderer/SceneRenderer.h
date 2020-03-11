#pragma once

#include "Render/RenderCore.h"

class IRenderCommandContext;
class IRenderSystemUniformBuffer;
class CScene;

/**
 * Scene global data to give to shaders
 * Should be present in every shader !
 */
struct SSceneShaderGlobalData
{
	float Time;	
};

/**
 * Scene renderer base interface
 * Used to render a CScene
 */
class ISceneRenderer
{
public:
    virtual ~ISceneRenderer() = default;

    /**
     * Called before render, useful for resource creation
     */
    virtual void PreRender() = 0;

	/**
	 * Render a scene
	 * Should be called by the render thread
	 */
	virtual void Render(IRenderCommandContext* InCommandContext,
		CScene* InScene, const SViewport& InViewport) = 0;
};