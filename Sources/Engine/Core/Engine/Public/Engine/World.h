#pragma once

#include "EngineCore.h"

namespace ZE
{

namespace ECS { class CEntityManager; }
namespace Renderer { class CWorldProxy; }

/**
 * A world
 * Contains a entity manager
 */
class CWorld final
{
public:
    CWorld();
    ~CWorld();

    void Tick(float InDeltaTime);

    ECS::CEntityManager* GetEntityManager() const { return EntityManager.get(); }
    Renderer::CWorldProxy* GetProxy() const { return Proxy.get(); }
private:
    std::unique_ptr<ECS::CEntityManager> EntityManager;
    std::unique_ptr<Renderer::CWorldProxy> Proxy;
};

} /* namespace ZE */