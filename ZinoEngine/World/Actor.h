#pragma once

#include "Core/EngineCore.h"
#include "Core/TickableObject.h"
#include "Components/ActorComponent.h"
#include "Components/WorldComponent.h"

/**
 * Base class for Actors, objects in the world
 */
ZCLASS()
class CActor : public ITickableObject
{
public:
	CActor() {}
	virtual ~CActor() = default;

	virtual void Tick(float InDeltaTime) override {}
	virtual void Destroy();

	/**
	 * Set actor transform
	 */
	void SetTransform(const STransform& InTransform);

	/**
	 * Get all actor components by class
	 */
	template<typename T>
	std::vector<std::weak_ptr<T>> GetComponentsByClass() const
	{
		std::vector<std::weak_ptr<T>> Components;

		for (const auto& Component : ActorComponents)
		{
			if (Component.second->get_type() == rttr::type::get<T>())
			{
				Components.push_back(std::static_pointer_cast<T>(Component.second));
			}
		}

		return Components;
	}
	/**
	 * Create new component and add it to actor components list
	 */
	template<typename T, typename... Args>
	std::shared_ptr<T> CreateComponent(const std::string& InID, Args&&... InArgs)
	{
		ActorComponents.insert(std::make_pair(InID, std::make_shared<T>(InArgs...)));
		ActorComponents[InID]->Owner = this;
		ActorComponents[InID]->Initialize();
		return std::static_pointer_cast<T>(ActorComponents[InID]);
	}

	const STransform& GetTransform() const { return RootComponent.lock()->GetTransform(); }
	const std::weak_ptr<CWorldComponent>& GetRootComponent() const { return RootComponent; }
protected:
	/** Root component, containing other world components */
	std::weak_ptr<CWorldComponent> RootComponent;

	/** Actor components, including world components */
	std::map<std::string, std::shared_ptr<CActorComponent>> ActorComponents;
};