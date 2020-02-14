#pragma once

#include "Core/TickableObject.h"

class CActor;

ZCLASS()
class CActorComponent : public ITickableObject
{
	REFLECTED_CLASS()

public:
	virtual ~CActorComponent() = default;

	virtual void Initialize() {}
	virtual void Tick(float InDeltaTime) override {}
	virtual void Destroy() {}

	CActor* GetOwner() const { return Owner; }
private:
	ZPROPERTY()
	CActor* Owner;
	
	friend class CActor;
};