#pragma once

#include "EngineCore.h"

namespace ZE
{

namespace ECS { class CEntityManager; }

/**
 * A world
 * Contains a entity manager
 */
class CWorld
{
public:
    CWorld();

    void Tick(float InDeltaTime);

    ECS::CEntityManager* GetEntityManager() const { return EntityManager.get(); }
private:
    std::unique_ptr<ECS::CEntityManager> EntityManager;
};

} /* namespace ZE */