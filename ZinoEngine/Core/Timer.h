#pragma once

#include "Core/EngineCore.h"

/** A timer ID */
using TimerID = uint64_t;

constexpr TimerID g_NullTimerID = -1;

/**
 * An timer entry
 */
struct STimerEntry
{
	TimerID ID;
	float CurrentTime;
	float Rate;
	bool bLoop;
	std::function<void(void)> Callback;

	STimerEntry() : ID(g_NullTimerID), CurrentTime(0.f) {}

	STimerEntry(const TimerID& InID, const float& InRate,
		const std::function<void(void)>& InCallback,
		const bool& bInLoop) : ID(InID),
		Rate(InRate), Callback(InCallback), bLoop(bInLoop), CurrentTime(0.f) {}
};

class CTimerManager
{
public:
	TimerID SetTimer(const float& InRate, const std::function<void(void)>& InCallback,
		const bool& bLoop = false);

	void UpdateTimers(const float& InDeltaTime);
private:
	std::map<TimerID, STimerEntry> Timers;
};