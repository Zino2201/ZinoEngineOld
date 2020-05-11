#include "Engine/Components/EventSystem.h"
#include "Reflection/Builders.h"

namespace ZE
{
namespace Refl
{

REFL_INIT_BUILDERS_FUNC(EventSystem)
{
	Builders::TClassBuilder<Components::CEventSystem>("CEventSystem")
		.Ctor<>();
}
}

namespace Components
{

void CEventSystem::Initialize(ECS::CEntityManager& InEntityManager)
{
	
}

void CEventSystem::Tick(ECS::CEntityManager& InEntityManager, const float& InDeltaTime)
{

}

} /** namespace Components */

}