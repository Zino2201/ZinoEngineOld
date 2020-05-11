#include "Engine/TickSystem.h"
#include <algorithm>

namespace ZE
{

void CTickSystem::Tick(ETickOrder InOrder, const float& InDeltaTime)
{
	CurrentOrder = InOrder;

	/**
	 * Check if we have any tickables waiting to be added to the global map
	 */
	if(!TickablesToAdd.empty())
	{
		for(auto& Tickable : TickablesToAdd)
		{
			auto& Tickables = TickablesMap[Tickable->GetTickOrder()];
			must(std::find(Tickables.begin(), Tickables.end(), Tickable) == Tickables.end());
			Tickables.push_back(Tickable);
		}

		TickablesToAdd.clear();
	}

	/**
	 * Tick the specified order and also ETickOrder::All
	 */
	auto& Tickables = TickablesMap[InOrder];
	for (auto& Tickable : Tickables)
	{
		Tickable->Tick(InDeltaTime);
	}

	Tickables = TickablesMap[ETickOrder::All];
	for (auto& Tickable : Tickables)
	{
		Tickable->Tick(InDeltaTime);
	}
}

void CTickSystem::Register(CTickable& InTickable)
{
	/** Add tickable next tick, we can't add it now as the virtual table isn't ready */
	TickablesToAdd.push_back(&InTickable);
}

void CTickSystem::Unregister(CTickable& InTickable)
{
	for(auto& [Order, Tickables] : TickablesMap)
		Tickables.erase(std::remove(Tickables.begin(), Tickables.end(), &InTickable),
			Tickables.end());
}

CTickable::CTickable() { CTickSystem::Get().Register(*this); }
CTickable::~CTickable() { CTickSystem::Get().Unregister(*this); }

};