#include "Engine/World.h"
#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ZE
{

CWorld::CWorld()
	: Proxy(std::make_unique<Renderer::CWorldProxy>())
{

}

CWorld::~CWorld() = default;

}