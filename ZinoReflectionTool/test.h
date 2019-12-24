#pragma once

#include "Core/TickableObject.h"

class CActor;

ZCLASS()
class CActorComponent : public ITickableObject
{
	REFLECTED_CLASS()

public:
	virtual ~CActorComponent() = default;

	virtual void Tick(float InDeltaTime) override {}

	CActor* GetOwner() const { return Owner; }
private:
	ZPROPERTY()
	CActor* Owner;
	
	ZPROPERTY()
	CActor* Owner;
	ZPROPERTY()
	CActor* Owner;
	
	friend class CActor;
};