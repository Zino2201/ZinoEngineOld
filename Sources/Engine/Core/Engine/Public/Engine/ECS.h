#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include "Module/Module.h"
#include <unordered_set>
#include "Pool.h"
#include "TickSystem.h"
#include "NonCopyable.h"
#include "ECS.gen.h"

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
class ENGINE_API CECSManager final : public CNonCopyable
{
public:
	static CECSManager& Get()
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

    template<typename T>
    T* GetSystem()
    {
        for(auto& System : Systems)
        {
            if(T* SystemPtr = Cast<T>(System.get()))
                return SystemPtr;
        }

        return nullptr;
    }
private:
    void OnModuleLoaded(const std::string_view& InName);
private:
    std::vector<std::unique_ptr<ECS::IEntityComponentSystem>> Systems;
    std::unordered_map<ETickOrder, std::vector<ECS::IEntityComponentSystem*>> GroupMap;
    std::unordered_set<Refl::CClass*> AddedSystems;
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
    CEntity(CEntityManager& InManager,
        const EntityID& InID) : Manager(InManager), ID(InID) {}

    ENGINE_API void AddComponent(SEntityComponent* InComponent);
    ENGINE_API SEntityComponent* RemoveComponent(ZE::Refl::CStruct* InComponent);

    const std::vector<SEntityComponent*>& GetComponents() const { return Components; }
private:
    EntityID ID;
    std::vector<SEntityComponent*> Components;
    CEntityManager& Manager;
};

/**
 * A entity component
 */
ZSTRUCT()
struct SEntityComponent
{
    REFL_BODY()

    virtual ~SEntityComponent() = default;

    ECS::EntityID ParentEntity;
};

/**
 * Component storing hierarchy data
 */
ZSTRUCT()
struct SHierarchyComponent : public ECS::SEntityComponent
{
    REFL_BODY()

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

/**
 * Base interface for entity component system
 */
ZCLASS()
class IEntityComponentSystem
{
    REFL_BODY()

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
    virtual ETickOrder GetOrder() const { return ETickOrder::StartOfFrame; }
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
    ENGINE_API SEntityComponent* AddComponent(
        const ECS::EntityID& InEntity,
        ZE::Refl::CStruct* InStruct);

    template<typename T>
	T* AddComponent(const ECS::EntityID& InEntity)
    {
        return Cast<T>(AddComponent(InEntity, Refl::CStruct::Get<T>()));
    }

    /**
     * Remove a component from the entity
     */
	ENGINE_API void RemoveComponent(
		const ECS::EntityID& InEntity,
		ZE::Refl::CStruct* InComponent);

    /**
     * Get the specified component of an entity
     */
    ENGINE_API SEntityComponent* GetComponent(const EntityID& InID,
        Refl::CStruct* InComponent);

    template<typename T>
    T* GetComponent(const EntityID& InID)
    {
        return Cast<T>(GetComponent(InID, Refl::CStruct::Get<T>()));
    }

    CEntity* TryGetEntityByID(const EntityID& InID) 
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
    ENGINE_API SEntityComponent* CreateComponent(ZE::Refl::CStruct* InStruct);
private:
    CWorld& World;

    std::unordered_map<EntityID, CEntity> Entities;

    /** Pools of components */
    std::unordered_map<ZE::Refl::CStruct*, 
        TDynamicPool<TComponentPool>> ComponentPoolMap;

    /** Map to a set of components */
    std::unordered_map<ZE::Refl::CStruct*, std::vector<ECS::SEntityComponent*>> ComponentVecMap;

    /** Available entity id */
    EntityID AvailableEntityID;

    TMulticastDelegate<CEntityManager&, const ECS::EntityID&, ECS::SEntityComponent*> OnComponentAdded;
    TMulticastDelegate<CEntityManager&, const ECS::EntityID&, ECS::SEntityComponent*> OnComponentRemoved;
};

}
