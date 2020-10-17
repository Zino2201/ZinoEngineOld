#include "Engine/World.h"
#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ze
{

CWorld::CWorld()
	: Proxy(std::make_unique<renderer::CWorldProxy>())
{

}

CWorld::~CWorld() = default;

}