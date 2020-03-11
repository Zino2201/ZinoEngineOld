#include "Timer.h"
#include <iostream>

TimerID CTimerManager::SetTimer(const float& InRate, const std::function<void(void)>& InCallback,
	const bool& bInLoop)
{	
	/** Generate ID */
	TimerID ID = rand();
	Timers[ID] = STimerEntry(ID, InRate, InCallback, bInLoop);
	return ID;
}

void CTimerManager::UpdateTimers(const float& InDeltaTime)
{
	for(auto& [ID, Timer] : Timers)
	{
		Timer.CurrentTime += InDeltaTime;

		if(Timer.CurrentTime >= Timer.Rate)
		{
			Timer.Callback();
			if(Timer.bLoop)
			{
				Timer.CurrentTime = 0.f;
			}
			else
			{
				Timers.erase(ID);
			}
		}
	}
}