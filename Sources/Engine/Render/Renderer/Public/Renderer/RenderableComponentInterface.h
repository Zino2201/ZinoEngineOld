#pragma once

#include "EngineCore.h"

namespace ZE::Renderer
{

/**
 * Base interface for renderable components
 */
class RENDERER_API IRenderableComponent
{
public:
	virtual Math::STransform& GetTransform() const = 0;
};

}