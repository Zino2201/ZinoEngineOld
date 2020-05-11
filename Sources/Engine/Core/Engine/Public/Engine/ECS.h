#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include "Module/Module.h"
#include <unordered_set>
#include "Pool.h"
#include "TickSystem.h"
#include "NonCopyable.h"

/**
 * ZinoEngine Entity Component System implementation
 */
namespace ZE { class CWorld; }

namespace ZE::ECS
{

class IEntityComponentSystem; 

/**
 * ECS Manager
 * Manage systems
 */
class CECSManager final : public CNonCopyable
{
public:
	ENGINE_API static CECSManager& Get()
	{
        static CECSManager Instance;
		return Instance;
	}

    void Initialize();

	std::vector<std::unique_ptr<ECS::IEntityComponentSystem>>& GetSystems() { return Systems; }
    std::optional<std::vector<ECS::IEntityComponentSystem*>*> GetSystemsByOrder(ETickOrder InOrder)
    {
        auto& Set = GroupMap.find(InOrder);

        if(Set != GroupMap.end())
            return std::make_optional(&Set->second);

        return std::nullopt;
    }

private:
    ENGINE_API void OnModuleLoaded(const std::string_view& InName);
private:
    std::vector<std::unique_ptr<ECS::IEntityComponentSystem>> Systems;
    std::unordered_map<ETickOrder, std::vector<ECS::IEntityComponentSystem*>> GroupMap;
    std::unordered_set<TNonOwningPtr<Refl::CClass>> AddedSystems;
};

using EntityID = uint64_t;

constexpr EntityID Null = -1;

class CEntityManager;
struct SEntityComponent;

/**
 * A entity
 * TODO: Remove this class ?
 */
class CEntity final
{
    friend class CEntityManager;

public:
    CEntity(const TNonOwningPtr<CEntityManager>& InManager,
        const EntityID& InID) : Manager(InManager), ID(InID) {}

    ENGINE_API void AddComponent(const TNonOwningPtr<SEntityComponent>& InComponent);
    ENGINE_API SEntityComponent* RemoveComponent(ZE::Refl::CStruct* InComponent);

    const std::vector<TNonOwningPtr<SEntityComponent>>& GetComponents() const { return Components; }
private:
    EntityID ID;
    std::vector<TNonOwningPtr<SEntityComponent>> Components;
    TNonOwningPtr<CEntityManager> Manager;
};

/**
 * A entity component
 */
struct SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS(SEntityComponent)

    virtual ~SEntityComponent() = default;
};

/**
 * Component storing hierarchy data
 */
struct SHierarchyComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(SHierarchyComponent, SEntityComponent)

    uint32_t ChildrenCount;

    /** ID of the parent of this entity */
    ECS::EntityID Parent;

    /** The first entity of this hierarchy */
    ECS::EntityID First;
    
    /** The previous entity relative to the parent's hierarchy */
    ECS::EntityID Previous;

    /** The previous entity relative to the parent's hierarchy */
    ECS::EntityID Next;

    /** The last entity of this hierarchy */
    ECS::EntityID Last;

    SHierarchyComponent() : ChildrenCount(0),
        Parent(ECS::Null),
        First(ECS::Null),
        Previous(ECS::Null),
        Next(ECS::Null),
        Last(ECS::Null) {}

};
DECLARE_REFL_TYPE(SHierarchyComponent);

/**
 * Base interface for entity component system
 */
class IEntityComponentSystem
{
    DECLARE_REFL_STRUCT_OR_CLASS(IEntityComponentSystem)

public:
    /**
     * Called when a entity manager is created
     */
    virtual void Initialize(CEntityManager& InEntityManager) = 0;

    /**
     * Called when the specified entity manager ticks
     */
    virtual void Tick(CEntityManager& InEntityManager, const float& InDeltaTime) = 0;

    // TODO: Remove ?
    virtual ZE::Refl::CStruct* GetComponentStruct() const = 0;

    /**
     * Determine if the entity component system will can tick
     */
    virtual bool ShouldTick() const = 0;

    /**
     * Get system priority
     */
    virtual uint32_t GetPriority() const = 0;

    /**
     * System group
     */
    ETickOrder GetOrder() const { return ETickOrder::StartOfFrame; }
};

/**
 * An entity manager
 * Manages entities and components
 * You can have multiple managers
 * Each manager must have a world
 */
class CEntityManager final : public CTickable,
    public CNonCopyable
{
    using TComponentPool = SEntityComponent;

public:
    ENGINE_API CEntityManager(CWorld& InWorld);

    /**
     * Create an entity
     */
    ENGINE_API EntityID CreateEntity();

    /**
     * Tick the components
     */
    ENGINE_API void Tick(const float& InDeltaTime) override;

    /**
     * Add a new component
     * If a component of the same type already exists, it returns it
     */
    ENGINE_API TNonOwningPtr<SEntityComponent> AddComponent(
        const ECS::EntityID& InEntity,
        const TNonOwningPtr<ZE::Refl::CStruct>& InStruct);

    /**
     * Remove a component from the entity
     */
	ENGINE_API void RemoveComponent(
		const ECS::EntityID& InEntity,
		const TNonOwningPtr<ZE::Refl::CStruct>& InComponent);

    /**
     * Get the specified component of an entity
     */
    ENGINE_API TNonOwningPtr<SEntityComponent> GetComponent(const EntityID& InID,
        const TNonOwningPtr<Refl::CStruct>& InComponent);

    template<typename T>
    TNonOwningPtr<T> GetComponent(const EntityID& InID)
    {
        return Cast<T>(GetComponent(InID, Refl::CStruct::Get<T>()));
    }

    TNonOwningPtr<CEntity> TryGetEntityByID(const EntityID& InID) 
    { 
        auto& It = Entities.find(InID);
        if(It != Entities.end())
            return &It->second;

        return nullptr;
    }

    /**
     * Attach a entity to another entity
     */
    ENGINE_API void AttachEntity(const EntityID& InEntity, const EntityID& InParent);

    ETickOrder GetTickOrder() const override { return ETickOrder::All; }
    CWorld& GetWorld() { return World; }
    CEntity& GetEntityByID(const EntityID& InID) { return Entities.find(InID)->second; }
    auto& GetOnComponentAdded() { return OnComponentAdded; }
    auto& GetOnComponentRemoved() { return OnComponentRemoved; }
    std::vector<ECS::SEntityComponent*>* GetComponents(ZE::Refl::CStruct* InStruct)
    {
        auto& It = ComponentVecMap.find(InStruct);

        if(It != ComponentVecMap.end())
            return &It->second;
        else
            return nullptr;
    }
private:
	ENGINE_API EntityID GetFreeID();
    ENGINE_API TNonOwningPtr<SEntityComponent> CreateComponent(const TNonOwningPtr<ZE::Refl::CStruct>& InStruct);
private:
    CWorld& World;

    std::unordered_map<EntityID, CEntity> Entities;

    /** Pools of components */
    std::unordered_map<TNonOwningPtr<ZE::Refl::CStruct>, 
        TPool<TComponentPool>> ComponentPoolMap;

    /** Map to a set of components */
    std::unordered_map<ZE::Refl::CStruct*, std::vector<ECS::SEntityComponent*>> ComponentVecMap;

    /** Available entity id */
    EntityID AvailableEntityID;

    TMulticastDelegate<CEntityManager&, const ECS::EntityID&, ECS::SEntityComponent*> OnComponentAdded;
    TMulticastDelegate<CEntityManager&, const ECS::EntityID&, ECS::SEntityComponent*> OnComponentRemoved;
};

DECLARE_REFL_TYPE(SEntityComponent);
DECLARE_REFL_TYPE(IEntityComponentSystem);

}
