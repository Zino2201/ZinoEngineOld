#include "ECS.h"
#include "Module/ModuleManager.h"
#include "Reflection/Builders.h"

DECLARE_LOG_CATEGORY(ECS);

namespace ZE
{

DEFINE_MODULE(CECSModule, "ECS");

namespace Refl
{
REFL_INIT_BUILDERS_FUNC()
{
	Refl::Builders::TStructBuilder<ECS::SEntityComponent>("SEntityComponent");
	Refl::Builders::TClassBuilder<ECS::IEntityComponentSystem>("IEntityComponentSystem")
		.MarkAsInterface();
}
} /** namespace Refl */

void CECSModule::Initialize()
{
	/** Hook to OnModuleLoaded, so we can load new systems when a module is loaded */
	CModuleManager::GetOnModuleLoadedDelegate()
		.Bind(std::bind(&CECSModule::OnModuleLoaded, this, std::placeholders::_1));
}	

void CECSModule::OnModuleLoaded(const std::string_view& InName)
{
	using namespace ECS;

	/** Search all IEntityComponentSystems */
	auto& SystemClasses = Refl::GetDerivedClassesFrom(
		Refl::CClass::Get<IEntityComponentSystem>());

	for(const auto& SystemClass : SystemClasses)
	{
		if(AddedSystems.count(SystemClass) || SystemClass->IsInterface())
			continue;

		IEntityComponentSystem* System = SystemClass->Instantiate<IEntityComponentSystem>();
		if(!System->GetComponentStruct())
		{
			delete System;
			continue;
		}

		Systems.push_back(std::unique_ptr<IEntityComponentSystem>(System));
		AddedSystems.insert(SystemClass);

		LOG(ELogSeverity::Info, ECS, "Instanciating entity component system %s for component %s", 
			SystemClass->GetName(), System->GetStruct()->GetName());
	}
}

namespace ECS
{

/** CEntity */
void CEntity::AddComponent(const TNonOwningPtr<ZE::Refl::CStruct>& InComponent)
{
	TNonOwningPtr<SEntityComponent> Component = Manager->CreateComponent(InComponent);
	Components.push_back(Component);
}

/** CEntityManager */

void CEntityManager::Tick(float InDeltaTime)
{
	for(auto& System : CECSModule::Get().GetSystems())
	{
		System->Tick(this, InDeltaTime);
	}
}

EntityID CEntityManager::CreateEntity()
{
	EntityID ID = GetFreeID();

	auto It = Entities.insert({ ID, CEntity(this, ID) });
	
	LOG(ELogSeverity::Info, ECS, "Created new entity with ID %i", ID);

	return ID;
}

SEntityComponent* CEntityManager::CreateComponent(const TNonOwningPtr<ZE::Refl::CStruct>& InStruct)
{
	SEntityComponent* Component = InStruct->Instantiate<SEntityComponent>();
	Components.push_back(std::unique_ptr<SEntityComponent>(Component));
	return Component;
}

SEntityComponent* CEntityManager::GetComponent(const EntityID& InID,
	const TNonOwningPtr<Refl::CStruct>& InComponent)
{
	auto& It = Entities.find(InID);

	if(It != Entities.end())
	{
		CEntity& Entity = It->second;

		for(auto& Component : Entity.Components)
		{
			if(Component->GetStruct() == InComponent)
				return Component;
		}
	}

	LOG(ELogSeverity::Info, ECS, "Failed to find component %s in entity ID %d",
		InComponent->GetName(), InID);

	return nullptr;
}

EntityID CEntityManager::GetFreeID()
{
	return AvailableEntityID++;
}

} /** namespace ECS */

} /** namespace ZE */