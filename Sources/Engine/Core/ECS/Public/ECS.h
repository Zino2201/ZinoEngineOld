#pragma once

#include "EngineCore.h"
#include "Reflection/Reflection.h"
#include "Module/Module.h"
#include <unordered_set>

/**
 * ZinoEngine Entity Component System implementation
 */
namespace ZE
{

namespace ECS { class IEntityComponentSystem; }

/**
 * ECS Module
 * Manage systems
 */
class CECSModule : public CModule
{
public:
	ECS_API static CECSModule& Get()
	{
		static CECSModule Instance;
		return Instance;
	}

    void Initialize() override;

	std::vector<std::unique_ptr<ECS::IEntityComponentSystem>>& GetSystems() { return Systems; }
private:
    void OnModuleLoaded(const std::string_view& InName);
private:
    std::vector<std::unique_ptr<ECS::IEntityComponentSystem>> Systems;
    std::unordered_set<TNonOwningPtr<Refl::CClass>> AddedSystems;
};

namespace ECS
{

using EntityID = uint32_t;

constexpr EntityID GNullEntityID = -1;

class CEntityManager;
struct SEntityComponent;

/**
 * A entity
 */
class CEntity
{
    friend class CEntityManager;

public:
    CEntity(const TNonOwningPtr<CEntityManager>& InManager,
        const EntityID& InID) : Manager(InManager), ID(InID) {}

    ECS_API void AddComponent(const TNonOwningPtr<ZE::Refl::CStruct>& InComponent);
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
};


/**
 * A entity component system
 */
class IEntityComponentSystem
{
    DECLARE_REFL_STRUCT_OR_CLASS(IEntityComponentSystem)

public:
    virtual void Tick(CEntityManager* InEntityManager, float InDeltaTime) = 0;
    virtual ZE::Refl::CStruct* GetComponentStruct() const = 0;
};

/**
 * An entity manager
 * Manages entities and components
 * You can have multiple managers
 */
class CEntityManager
{
public:
    /**
     * Create an entity
     */
    ECS_API EntityID CreateEntity();

    /**
     * Tick the components
     */
    ECS_API void Tick(float InDeltaTime);

    /**
     * Create a new component
     */
    ECS_API TNonOwningPtr<SEntityComponent> CreateComponent(const TNonOwningPtr<ZE::Refl::CStruct>& InStruct);

    /**
     * Get the specified component of an entity
     */
    ECS_API TNonOwningPtr<SEntityComponent> GetComponent(const EntityID& InID,
        const TNonOwningPtr<Refl::CStruct>& InComponent);

    template<typename T>
    TNonOwningPtr<T> GetComponent(const EntityID& InID)
    {
        return Cast<T>(GetComponent(Refl::CStruct::Get<T>()));
    }

    CEntity& GetEntityByID(const EntityID& InID) { return Entities.find(InID)->second; }
private:
	ECS_API EntityID GetFreeID();
private:
    std::unordered_map<EntityID, CEntity> Entities;

    /** The component vector (TODO make a pool) */
    std::vector<std::unique_ptr<SEntityComponent>> Components;

    /** Available entity id */
    EntityID AvailableEntityID;
};

DECLARE_REFL_TYPE(SEntityComponent);
DECLARE_REFL_TYPE(IEntityComponentSystem);

}
}
