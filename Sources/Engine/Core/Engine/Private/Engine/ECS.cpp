#include "Engine/ECS.h"
#include "Module/ModuleManager.h"
#include "Reflection/Builders.h"

namespace ZE::ECS
{

void CECSManager::Initialize()
{
	/** Hook to OnModuleLoaded, so we can load new systems when a module is loaded */
	ZE::Module::GetOnModuleLoadedDelegate()
		.Bind(std::bind(&CECSManager::OnModuleLoaded, this, std::placeholders::_1));

	OnModuleLoaded("Engine");
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

		ZE::Logger::Info("Instanciating entity component system {} for component {}", 
			SystemClass->GetName(), System->GetStaticClass()->GetName());
	}

	/** Sort */
	// TODO: IMPLEMENT SORTING !
}

/** CEntity */
void CEntity::AddComponent(SEntityComponent* InComponent)
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
	TickFlags = ETickFlagBits::Variable | ETickFlagBits::Fixed | ETickFlagBits::EndOfSimulation;

	for(const auto& System : CECSManager::Get().GetSystems())
	{
		System->Initialize(*this);
	}
}

void CEntityManager::Tick(const float& InDeltaTime)
{
	auto Systems = CECSManager::Get().GetSystemsByOrder(ETickFlagBits::Variable);

	if (Systems.has_value())
	{
		for (auto& System : *Systems.value())
		{
			if (System->ShouldTick())
				System->Tick(*this, InDeltaTime);
		}
	}
}

void CEntityManager::FixedTick(const float& InDeltaTime)
{
	auto Systems = CECSManager::Get().GetSystemsByOrder(ETickFlagBits::Fixed);

	if (Systems.has_value())
	{
		for (auto& System : *Systems.value())
		{
			if (System->ShouldTick())
				System->Tick(*this, InDeltaTime);
		}
	}
}

void CEntityManager::LateTick(const float& InDeltaTime)
{
	auto Systems = CECSManager::Get().GetSystemsByOrder(ETickFlagBits::EndOfSimulation);

	if (Systems.has_value())
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

	auto It = Entities.insert({ ID, CEntity(*this, ID) });
	
	ZE::Logger::Verbose("Created new entity with ID {}", ID);

	return ID;
}

SEntityComponent* CEntityManager::AddComponent(
	const ECS::EntityID& InEntity,
	ZE::Refl::CStruct* InStruct)
{
	must(InStruct);

	CEntity* Entity = TryGetEntityByID(InEntity);
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

		Component->ParentEntity = InEntity;

		if(!ComponentVecMap.count(InStruct))
			ComponentVecMap.insert({ InStruct, {} });

		ComponentVecMap[InStruct].push_back(Component);

		OnComponentAdded.Broadcast(*this, InEntity, Component);

		ZE::Logger::Verbose("Added component {} to entity ID {}", 
			InStruct->GetName(), InEntity);

		return Component;
	}

	ZE::Logger::Error("Failed to add component {} to entity ID {} (entity doesn't exist)",
		InStruct->GetName(), InEntity);
	
	return nullptr;
}

void CEntityManager::RemoveComponent(
	const ECS::EntityID& InEntity,
	ZE::Refl::CStruct* InComponent)
{
	must(InComponent);

	CEntity* Entity = TryGetEntityByID(InEntity);
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

		ZE::Logger::Verbose("Removed component {} to entity ID {}",
			InComponent->GetName(), InEntity);
	}
	else
	{
		ZE::Logger::Error("Failed to remove component {} in entity ID {} (entity doesn't exist)",
			InComponent->GetName(), InEntity);
	}
}

SEntityComponent* CEntityManager::CreateComponent(ZE::Refl::CStruct* InStruct)
{
	if(!ComponentPoolMap.count(InStruct))
		ComponentPoolMap.insert(std::make_pair(InStruct, 
			TDynamicPool<TComponentPool>(100, InStruct->GetSize())));

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
		ZE::Logger::Error("Failed to attach entity {} to {}, one is missing hierarchy component",
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
	Refl::CStruct* InComponent)
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

	ZE::Logger::Info("Failed to find component {} in entity ID {}",
		InComponent->GetName(), InID);

	return nullptr;
}

EntityID CEntityManager::GetFreeID()
{
	return AvailableEntityID++;
}

} /** namespace ZE::ECS */