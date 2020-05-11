#pragma once

#include "Engine/ECS.h"

namespace ZE::Components
{

/**
 *
 */
class CEventSystem : public ECS::IEntityComponentSystem
{
	DECLARE_REFL_STRUCT_OR_CLASS1(CEventSystem, IEntityComponentSystem)

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
DECLARE_REFL_TYPE(CEventSystem);

}