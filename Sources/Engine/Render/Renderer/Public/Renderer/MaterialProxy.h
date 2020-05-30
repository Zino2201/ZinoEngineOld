#pragma once

#include "EngineCore.h"
#include "Render/RenderThreadResource.h"

namespace ZE::Renderer
{

/**
 * Shader-map of a material 
 */   
class CMaterialShaderMap
{
};

/**
 * Render-thread version of a material
 */
class CMaterialProxy : public CRenderThreadResource
{
private:
    CMaterialShaderMap ShaderMap;
};

}