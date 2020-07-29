#include "Engine/TickSystem.h"
#include <algorithm>
#include "Engine/Engine.h"

namespace ZE
{

void CTickSystem::Tick(ETickFlagBits InFlag, const float& InDeltaTime)
{
	CurrentTick = InFlag;

	/**
	 * Check if we have any tickables waiting to be added to the global map
	 */
	if(!TickablesToAdd.empty())
	{
		for(auto& Tickable : TickablesToAdd)
		{
			if(!Tickable->CanEverTick())
				continue;

			// TODO: Find a way to iterate over flags
			
			if(Tickable->GetTickFlags() & ETickFlagBits::Variable)
			{
				auto& Tickables = TickablesMap[ETickFlagBits::Variable];
				must(std::find(Tickables.begin(), Tickables.end(), Tickable) == Tickables.end());
				Tickables.push_back(Tickable);
			}
			
			if(Tickable->GetTickFlags() & ETickFlagBits::Fixed)
			{
				auto& Tickables = TickablesMap[ETickFlagBits::Fixed];
				must(std::find(Tickables.begin(), Tickables.end(), Tickable) == Tickables.end());
				Tickables.push_back(Tickable);
			}

			if(Tickable->GetTickFlags() & ETickFlagBits::EndOfSimulation)
			{
				auto& Tickables = TickablesMap[ETickFlagBits::EndOfSimulation];
				must(std::find(Tickables.begin(), Tickables.end(), Tickable) == Tickables.end());
				Tickables.push_back(Tickable);
			}	
		}

		TickablesToAdd.clear();
	}

	switch(InFlag)
	{
	case ETickFlagBits::Variable:
	{
		auto& Tickables = TickablesMap[ETickFlagBits::Variable];
		for (auto& Tickable : Tickables)
		{
			Tickable->Tick(InDeltaTime);
		}
		break;
	}
	case ETickFlagBits::Fixed:
	{
		auto& Tickables = TickablesMap[ETickFlagBits::Fixed];
		for (auto& Tickable : Tickables)
		{
			Tickable->FixedTick(InDeltaTime);
		}
		break;
	}
	case ETickFlagBits::EndOfSimulation:
	{	
		auto& Tickables = TickablesMap[ETickFlagBits::EndOfSimulation];
		for (auto& Tickable : Tickables)
		{
			Tickable->LateTick(InDeltaTime);
		}
		break;
	}
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

CTickable::CTickable() : bCanEverTick(true), 
	bIsTickEnabled(true), TickFlags(ETickFlagBits::Variable) { CZinoEngineApp::Get()->GetTickSystem().Register(*this); }
CTickable::~CTickable() { CZinoEngineApp::Get()->GetTickSystem().Unregister(*this); }

};