#pragma once

#include "Core/EngineCore.h"
#include "Core/TickableObject.h"

class CActor;

/**
 * The world
 * Contains list of actors
 */
class CWorld : public ITickableObject
{
public:
	CWorld();
	~CWorld();

	virtual void Tick(float InDeltaTime) override;

	template<typename T, typename... Args> 
	std::shared_ptr<T> SpawnActor(const STransform& InTransform = STransform(), Args&&... InArgs)
	{
		Actors.push_back(std::make_shared<T>(Args...));
		assert(!Actors.back()->GetRootComponent().expired() && "Root component should not be nullptr");
		Actors.back()->SetTransform(InTransform);
		return std::static_pointer_cast<T>(Actors.back());
	}

	std::vector<std::shared_ptr<CActor>> GetActors() const { return Actors; }
private:
	std::vector<std::shared_ptr<CActor>> Actors;
};