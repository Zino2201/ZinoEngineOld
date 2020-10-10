#pragma once

#include "EngineCore.h"
#include "TickSystem.h"

namespace ZE
{

namespace Renderer { class CWorldProxy; }

/**
 * A world
 * Contains a entity manager
 */
class ENGINE_API CWorld final
{
public:
    CWorld();
    ~CWorld();

    Renderer::CWorldProxy* GetProxy() const { return Proxy.get(); }
private:
    std::unique_ptr<Renderer::CWorldProxy> Proxy;
};

} /* namespace ZE */