#pragma once

#include "EngineCore.h"

namespace ze::renderer
{

/**
 * Base interface for renderable components
 */
class RENDERER_API IRenderableComponent
{
public:
	virtual maths::Transform& GetTransform() const = 0;
};

}