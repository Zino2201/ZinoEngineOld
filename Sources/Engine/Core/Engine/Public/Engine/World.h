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
class ENGINE_API CWorld final : public CTickable
{
public:
    CWorld();
    ~CWorld();

    void Tick(const float& InDeltaTime) override;

    ECS::CEntityManager* GetEntityManager() const { return EntityManager.get(); }
    Renderer::CWorldProxy* GetProxy() const { return Proxy.get(); }

    ETickOrder GetTickOrder() const override { return ETickOrder::StartOfFrame; }
private:
    std::unique_ptr<ECS::CEntityManager> EntityManager;
    std::unique_ptr<Renderer::CWorldProxy> Proxy;
};

} /* namespace ZE */