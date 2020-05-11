#include "Engine/ECS.h"
#include "Module/ModuleManager.h"
#include "Reflection/Builders.h"

DECLARE_LOG_CATEGORY(ECS);

namespace ZE
{

namespace Refl
{
REFL_INIT_BUILDERS_FUNC(ECS)
{
	Refl::Builders::TStructBuilder<ECS::SEntityComponent>("SEntityComponent");
	Refl::Builders::TClassBuilder<ECS::IEntityComponentSystem>("IEntityComponentSystem")
		.MarkAsInterface();
	Refl::Builders::TStructBuilder<ECS::SHierarchyComponent>("SHierarchyComponent")
		.Ctor<>();
}
} /** namespace Refl */

namespace ECS
{

void CECSManager::Initialize()
{
	/** Hook to OnModuleLoaded, so we can load new systems when a module is loaded */
	CModuleManager::GetOnModuleLoadedDelegate()
		.Bind(std::bind(&CECSManager::OnModuleLoaded, this, std::placeholders::_1));
}	

void CECSManager::OnModuleLoaded(const std::string_view& InName)
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
		GroupMap[System->GetOrder()].push_back(System);

		LOG(ELogSeverity::Info, ECS, "Instanciating entity component system %s for component %s", 
			SystemClass->GetName(), System->GetStruct()->GetName());
	}

	/** Sort */
	// TODO: IMPLEMENT SORTING !
}

/** CEntity */
void CEntity::AddComponent(const TNonOwningPtr<SEntityComponent>& InComponent)
{
	Components.push_back(InComponent);
}

SEntityComponent* CEntity::RemoveComponent(ZE::Refl::CStruct* InComponent)
{
	size_t Idx = 0;
	for(const auto& Component : Components)
	{
		if(Component->GetStruct() == InComponent)
		{
			SEntityComponent* Comp = Component;
			Components.erase(Components.begin() + Idx);
			return Comp;
		}

		Idx++;
	}

	return nullptr;
}

/** CEntityManager */

CEntityManager::CEntityManager(CWorld& InWorld) : World(InWorld), AvailableEntityID(0)
{
	for(const auto& System : CECSManager::Get().GetSystems())
	{
		System->Initialize(*this);
	}
}

void CEntityManager::Tick(const float& InDeltaTime)
{
	auto Systems = CECSManager::Get().GetSystemsByOrder(CTickSystem::Get().GetCurrentOrder());

	if(Systems.has_value())
	{
		for (auto& System : *Systems.value())
		{
			if (System->ShouldTick())
				System->Tick(*this, InDeltaTime);
		}
	}
}

EntityID CEntityManager::CreateEntity()
{
	EntityID ID = GetFreeID();

	auto It = Entities.insert({ ID, CEntity(this, ID) });
	
	LOG(ELogSeverity::Debug, ECS, "Created new entity with ID %i", ID);

	return ID;
}

TNonOwningPtr<SEntityComponent> CEntityManager::AddComponent(
	const ECS::EntityID& InEntity,
	const TNonOwningPtr<ZE::Refl::CStruct>& InStruct)
{
	must(InStruct);

	TNonOwningPtr<CEntity> Entity = TryGetEntityByID(InEntity);
	if(Entity)
	{
		/**
		 * Don't add if the entity already has the component
		 */
		for(const auto& Component : Entity->GetComponents())
			if(Component->GetStruct() == InStruct)
				return Component;

		SEntityComponent* Component = CreateComponent(InStruct);
		Entity->AddComponent(Component);

		if(!ComponentVecMap.count(InStruct))
			ComponentVecMap.insert({ InStruct, {} });

		ComponentVecMap[InStruct].push_back(Component);

		OnComponentAdded.Broadcast(*this, InEntity, Component);

		LOG(ELogSeverity::Debug, ECS, "Added component %s to entity ID %i", 
			InStruct->GetName(), InEntity);

		return Component;
	}

	LOG(ELogSeverity::Error, ECS, 
		"Failed to add component %s to entity ID %d (entity doesn't exist)",
		InStruct->GetName(), InEntity);
	
	return nullptr;
}

void CEntityManager::RemoveComponent(
	const ECS::EntityID& InEntity,
	const TNonOwningPtr<ZE::Refl::CStruct>& InComponent)
{
	must(InComponent);

	TNonOwningPtr<CEntity> Entity = TryGetEntityByID(InEntity);
	if (Entity)
	{
		SEntityComponent* Component = Entity->RemoveComponent(InComponent);
		OnComponentRemoved.Broadcast(*this, InEntity, Component);

		/** Remove from component vec map */
		{
			auto& ComponentVec = ComponentVecMap[InComponent];
			size_t Idx = 0;
			for (const auto& Comp : ComponentVec)
			{
				if (Comp == Component)
				{
					ComponentVec.erase(ComponentVec.begin() + Idx);
					break;
				}

				Idx++;
			}
		}

		/** Free the memory */
		ComponentPoolMap[InComponent].Free(*Component);

		LOG(ELogSeverity::Debug, ECS, "Removed component %s to entity ID %i",
			InComponent->GetName(), InEntity);
	}
	else
	{
		LOG(ELogSeverity::Error, ECS,
			"Failed to remove component %s in entity ID %d (entity doesn't exist)",
			InComponent->GetName(), InEntity);
	}
}

SEntityComponent* CEntityManager::CreateComponent(const TNonOwningPtr<ZE::Refl::CStruct>& InStruct)
{
	if(!ComponentPoolMap.count(InStruct))
		ComponentPoolMap.insert(std::make_pair(InStruct, 
			TPool<TComponentPool>(100, InStruct->GetSize())));

	SEntityComponent* Component = ComponentPoolMap[InStruct].Allocate();
	InStruct->PlacementNew(Component);

	return Component;
}

void CEntityManager::AttachEntity(const EntityID& InEntity, const EntityID& InParent)
{
	/**
	 * Search for an hierachy componnt on both entities
	 */
	SHierarchyComponent* Parent = GetComponent<SHierarchyComponent>(InParent);
	SHierarchyComponent* Child = GetComponent<SHierarchyComponent>(InEntity);

	if(!Parent || !Child)
	{
		must(false);
		LOG(ELogSeverity::Error, ECS, 
			"Failed to attach entity %d to %d, one is missing hierarchy component",
			InEntity, InParent);
		return;
	}

	/** 
	 * Attach entity to parent
	 */
	if(Parent->Last != Null)
	{
		SHierarchyComponent* Last = GetComponent<SHierarchyComponent>(Parent->Last);
		
		Last->Next = InEntity;
	}
	
	Child->Parent = InParent;
	Child->Previous = Parent->Last;
	Parent->Last = InEntity;
	Parent->ChildrenCount++;

	if(Parent->First == Null)
		Parent->First = InEntity;
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