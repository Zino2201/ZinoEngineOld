#pragma once

#include "EngineCore.h"
#include "TickSystem.h"

namespace ze
{

namespace renderer { class CWorldProxy; }

/**
 * A world
 * Contains a entity manager
 */
class ENGINE_API CWorld final
{
public:
    CWorld();
    ~CWorld();

    renderer::CWorldProxy* GetProxy() const { return Proxy.get(); }
private:
    std::unique_ptr<renderer::CWorldProxy> Proxy;
};

} /* namespace ZE */