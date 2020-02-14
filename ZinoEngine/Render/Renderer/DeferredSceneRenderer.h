#pragma once

#include "SceneRenderer.h"

/**
 * Deferred scene renderer
 * Render geometry to a GBuffer and calculate lighting using gbuffer before rendering
 * This enable a massive number of lights compared to classic forward renderer
 */
class CDeferredSceneRenderer : public ISceneRenderer
{
};