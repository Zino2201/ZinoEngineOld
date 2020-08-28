#pragma once

#include "EngineCore.h"
#include "TickSystem.h"

namespace ZE
{

namespace ECS { class CEntityManager; }
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

    ECS::CEntityManager* GetEntityManager() const { return EntityManager.get(); }
    Renderer::CWorldProxy* GetProxy() const { return Proxy.get(); }
private:
    std::unique_ptr<ECS::CEntityManager> EntityManager;
    std::unique_ptr<Renderer::CWorldProxy> Proxy;
};

} /* namespace ZE */