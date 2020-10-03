#pragma once

#include "Engine/ECS.h"
#include "EventSystem.gen.h"

namespace ZE::Components
{

/**
 *
 */
ZCLASS()
class CEventSystem : public ECS::IEntityComponentSystem
{
	ZE_REFL_BODY()

public:
	void Initialize(ECS::CEntityManager& InEntityManager) override;
	void Tick(ECS::CEntityManager& InEntityManager, const float& InDeltaTime) override;
	ZE::Refl::CStruct* GetComponentStruct() const override
	{
		return nullptr;
	}
	bool ShouldTick() const override { return true; }
	uint32_t GetPriority() const override { return 0; }
};

}